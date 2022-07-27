# -*- coding: utf-8 -*-
import json
import re
import subprocess

import lhapdf
import numpy as np
import pandas as pd
import pineappl

from ... import configs, install, log, tools
from .. import interface
from . import paths


class Mg5(interface.External):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.user_cuts = []
        self.patches = []
        self.tau_min = None

    @property
    def mg5_dir(self):
        return self.dest / self.name

    @staticmethod
    def install():
        install.pineappl()
        install.mg5amc()

    @property
    def pdf_id(self):
        return lhapdf.mkPDF(self.pdf).info().get_entry("SetIndex")

    def run(self):
        # copy the output file to the directory and replace the variables
        output = (self.source / "output.txt").read_text().replace("@OUTPUT@", self.name)
        output_file = self.dest / "output.txt"
        output_file.write_text(output)

        # create output folder
        log.subprocess(
            [str(configs.configs["commands"]["mg5"]), str(output_file)],
            cwd=self.dest,
            out=(self.dest / "output.log"),
        )

        # copy patches if there are any; use xargs to properly signal failures
        for p in self.source.iterdir():
            if p.suffix == ".patch":
                subprocess.run(
                    "patch -p1".split(),
                    input=p.read_text(),
                    text=True,
                    cwd=self.mg5_dir,
                )

        # enforce proper analysis
        # - copy analysis.f
        analysis = (self.source / "analysis.f").read_text()
        (self.mg5_dir / "FixedOrderAnalysis" / f"{self.name}.f").write_text(analysis)
        # - update analysis card
        analysis_card = self.mg5_dir / "Cards" / "FO_analyse_card.dat"
        analysis_card.write_text(
            analysis_card.read_text().replace("analysis_HwU_template", self.name)
        )

        # copy the launch file to the directory and replace the variables
        launch = (self.source / "launch.txt").read_text().replace("@OUTPUT@", self.name)

        # TODO: write a list with variables that should be replaced in the
        # launch file; for the time being we create the file here, but in the
        # future it should be read from the theory database EDIT: now available
        # in self.theory
        variables = json.loads((paths.subpkg.parents[1] / "variables.json").read_text())
        variables["LHAPDF_ID"] = self.pdf_id

        # replace the variables with their values
        for name, value in variables.items():
            launch = launch.replace(f"@{name}@", str(value))

        # finally write launch
        launch_file = self.dest / "launch.txt"
        launch_file.write_text(launch)

        # parse launch file for user-defined cuts
        user_cuts_pattern = re.compile(
            r"^#user_defined_cut set (\w+)\s+=\s+([+-]?\d+(?:\.\d+)?|True|False)$"
        )
        for line in launch.splitlines():
            m = re.fullmatch(user_cuts_pattern, line)
            if m is not None:
                self.user_cuts.append((m[1], m[2]))

        # if there are user-defined cuts, implement them
        apply_user_cuts(self.mg5_dir / "SubProcesses" / "cuts.f", self.user_cuts)

        # parse launch file for user-defined minimum tau
        user_taumin_pattern = re.compile(r"^#user_defined_tau_min (.*)")
        user_taumin = None
        for line in launch.splitlines():
            m = re.fullmatch(user_taumin_pattern, line)
            if m is not None:
                try:
                    user_taumin = float(m[1])
                except ValueError:
                    raise ValueError("User defined tau_min is expected to be a number")

        if user_taumin is not None:
            set_tau_min_patch = (
                (paths.patches / "set_tau_min.patch")
                .read_text()
                .replace("@TAU_MIN@", f"{user_taumin}d0")
            )
            (self.dest / "set_tau_min.patch").write_text(set_tau_min_patch)
            self.tau_min = user_taumin
            tools.patch(set_tau_min_patch, self.mg5_dir)

        # parse launch file for other patches
        enable_patches_pattern = re.compile(r"^#enable_patch (.*)")
        enable_patches_list = []
        for line in launch.splitlines():
            m = re.fullmatch(enable_patches_pattern, line)
            if m is not None:
                enable_patches_list.append(m[1])

        if len(enable_patches_list) != 0:
            for patch in enable_patches_list:
                patch_file = paths.patches / patch
                patch_file = patch_file.with_suffix(patch_file.suffix + ".patch")
                if not patch_file.exists():
                    raise ValueError(
                        f"Patch '{patch}' requested, but does not exist in patches folder"
                    )
                self.patches.append(patch)
                tools.patch(patch_file.read_text(), self.mg5_dir)

        # launch run
        log.subprocess(
            [str(configs.configs["commands"]["mg5"]), str(launch_file)],
            cwd=self.dest,
            out=self.dest / "launch.log",
        )

    def generate_pineappl(self):
        # if rerunning without regenerating, let's remove the already merged
        # grid (it will be soon reobtained)
        if self.timestamp is not None:
            self.grid.unlink()

        # merge the final bins
        mg5_grids = sorted(
            str(p.absolute())
            for p in self.mg5_dir.glob("Events/run_01*/amcblast_obs_*.pineappl")
        )
        # read the first one from file
        grid = pineappl.grid.Grid.read(mg5_grids[0])
        # subsequently merge all the others (disk -> memory)
        for path in mg5_grids[1:]:
            grid.merge_from_file(path)

        # optimize the grids
        grid.optimize()

        # add results to metadata
        runcard = next(
            iter(self.mg5_dir.glob("Events/run_01*/run_01*_tag_1_banner.txt"))
        )
        grid.set_key_value("runcard", runcard.read_text())
        # add generated cards to metadata
        grid.set_key_value("output.txt", (self.dest / "output.txt").read_text())
        grid.set_key_value("launch.txt", (self.dest / "launch.txt").read_text())
        # add patches and cuts used to metadata
        grid.set_key_value("patches", "\n".join(self.patches))
        grid.set_key_value(
            "tau_min", str(self.tau_min) if self.tau_min is not None else ""
        )
        grid.set_key_value(
            "user_cuts", "\n".join(f"{var}={value}" for var, value in self.user_cuts)
        )

        grid.write(str(self.grid))

    def results(self):
        madatnlo = next(
            iter(self.mg5_dir.glob("Events/run_01*/MADatNLO.HwU"))
        ).read_text()
        table = filter(
            lambda line: re.match("^  [+-]", line) is not None, madatnlo.splitlines()
        )
        df = pd.DataFrame(
            np.array([[float(x) for x in line.split()] for line in table])
        )
        # start column from 1
        df.columns += 1
        df["result"] = df[3]
        df["error"] = df[4]
        df["sv_min"] = df[6]
        df["sv_max"] = df[7]

        return df

    def collect_versions(self):
        versions = {}
        versions["mg5amc_revno"] = (
            subprocess.run(
                "brz revno".split(),
                cwd=configs.configs["paths"]["mg5amc"],
                stdout=subprocess.PIPE,
            )
            .stdout.decode()
            .strip()
        )
        mg5amc_repo = (
            subprocess.run(
                "brz info".split(),
                cwd=configs.configs["paths"]["mg5amc"],
                stdout=subprocess.PIPE,
            )
            .stdout.decode()
            .strip()
        )

        repo = re.search(r"\s*parent branch:\s*(.*)", mg5amc_repo)
        if repo is None:
            print("Invalid mg5 repository")
        versions["mg5amc_repo"] = repo[1] if repo is not None else None
        return versions


def find_marker_position(insertion_marker, contents):
    marker_pos = -1

    for lineno, value in enumerate(contents):
        if insertion_marker in value:
            marker_pos = lineno
            break

    if marker_pos == -1:
        raise ValueError(
            "Error: could not find insertion marker `{insertion_marker}` in cut file `{file_path}`"
        )

    return marker_pos


def apply_user_cuts(cuts_file, user_cuts):
    """Apply a user defined cut, patching a suitable cuts file"""
    with open(cuts_file, "r") as fd:
        contents = fd.readlines()

    # insert variable declaration
    marker_pos = find_marker_position("logical function passcuts_user", contents)
    marker_pos = marker_pos + 8

    for fname in paths.cuts_variables.iterdir():
        name = fname.stem
        if any(i[0].startswith(name) for i in user_cuts):
            contents.insert(marker_pos, fname.read_text())

    marker_pos = find_marker_position("USER-DEFINED CUTS", contents)
    # skip some lines with comments
    marker_pos = marker_pos + 4
    # insert and empty line
    contents.insert(marker_pos - 1, "\n")

    for name, value in reversed(user_cuts):
        # map to fortran syntax
        if value == "True":
            value = ".true."
        elif value == "False":
            value = ".false."
        else:
            try:
                float(value)
            except ValueError:
                raise ValueError(f"Error: format of value `{value}` not understood")

            value = value + "d0"

        code = (paths.cuts_code / f"{name}.f").read_text().format(value)
        contents.insert(marker_pos, code)

    with open(cuts_file, "w") as fd:
        fd.writelines(contents)
