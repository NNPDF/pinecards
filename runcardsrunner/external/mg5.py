import json
import re
import subprocess

import numpy as np
import pandas as pd

from .. import install, log, paths, tools
from . import interface


class Mg5(interface.External):
    @property
    def mg5_dir(self):
        return self.dest / self.name

    @staticmethod
    def install():
        install.mg5amc()

    def run(self):
        # copy the output file to the directory and replace the variables
        output = (self.source / "output.txt").read_text().replace("@OUTPUT@", self.name)
        output_file = self.dest / "output.txt"
        output_file.write_text(output)

        # create output folder
        output_log = log.subprocess(
            [str(paths.mg5_exe), str(output_file)], dest=self.dest
        )
        (self.dest / "output.log").write_text(output_log)

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
        analysis = (paths.runcards / self.name / "analysis.f").read_text()
        (self.mg5_dir / "FixedOrderAnalysis" / f"{self.name}.f").write_text(analysis)
        # - update analysis card
        analysis_card = self.mg5_dir / "Cards" / "FO_analyse_card.dat"
        analysis_card.write_text(
            analysis_card.read_text().replace("analysis_HwU_template", self.name)
        )

        # copy the launch file to the directory and replace the variables
        launch = (self.source / "launch.txt").read_text().replace("@OUTPUT@", self.name)

        # TODO: write a list with variables that should be replaced in the launch file; for the time
        # being we create the file here, but in the future it should be read from the theory database
        # EDIT: now available in self.theory
        variables = json.loads((paths.pkg / "variables.json").read_text())

        # replace the variables with their values
        for name, value in variables.items():
            launch = launch.replace(f"@{name}@", value)

        # finally write launch
        launch_file = self.dest / "launch.txt"
        launch_file.write_text(launch)

        # parse launch file for user-defined cuts
        user_cuts_pattern = re.compile(
            r"^#user_defined_cut set (\w+)\s+=\s+([+-]?\d+(?:\.\d+)?|True|False)$"
        )
        user_cuts = []
        for line in launch.splitlines():
            m = re.fullmatch(user_cuts_pattern, line)
            if m is not None:
                user_cuts.append((m[1], m[2]))

        # if there are user-defined cuts, implement them
        apply_user_cuts(self.mg5_dir / "SubProcesses" / "cuts.f", user_cuts)

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
                tools.patch(patch_file.read_text(), self.mg5_dir)

        # launch run
        launch_log = log.subprocess(
            [str(paths.mg5_exe), str(launch_file)], dest=self.dest
        )
        (self.dest / "launch.log").write_text(launch_log)

    def results(self):
        madatnlo = next(
            iter(self.mg5_dir.glob("Events/run_01*/MADatNLO.HwU"))
        ).read_text()
        table = filter(
            lambda line: re.match("^  [+-]", line) is not None, madatnlo.splitlines()
        )
        df = pd.DataFrame(np.array([[float(x) for x in l.split()] for l in table]))
        # start column from 1
        df.columns += 1
        df["result"] = df[3]
        df["error"] = df[4]
        df["sv_min"] = df[6]
        df["sv_max"] = df[7]

        return df

    def generate_pineappl(self):
        pineappl = paths.pineappl_exe()

        # if rerunning without regenerating, let's remove the already merged
        # grid (it will be soon reobtained)
        if self.timestamp is not None:
            self.grid.unlink()

        # merge the final bins
        mg5_grids = " ".join(
            sorted(
                str(p)
                for p in self.mg5_dir.glob("Events/run_01*/amcblast_obs_*.pineappl")
            )
        )
        subprocess.run(f"{pineappl} merge {self.grid} {mg5_grids}".split())

        # optimize the grids
        subprocess.run(f"{pineappl} optimize {self.grid} {self.gridtmp}".split())
        self.update_with_tmp()

        # add metadata
        metadata = self.source / "metadata.txt"
        runcard = next(
            iter(self.mg5_dir.glob("Events/run_01*/run_01*_tag_1_banner.txt"))
        )
        entries = []
        if metadata.exists():
            for line in metadata.read_text().splitlines():
                k, v = line.split("=")
                entries += ["--entry", k, f"'{v}'"]
        subprocess.run(
            f"{pineappl} set {self.grid} {self.gridtmp}".split()
            + entries
            + f"--entry_from_file runcard {runcard}".split()
        )
        self.update_with_tmp()

        # find out which PDF set was used to generate the predictions
        pdf = re.search(r"set lhaid (\d+)", (self.dest / "launch.txt").read_text())[1]

        # (re-)produce predictions
        with open(self.dest / "pineappl.convolute", "w") as fd:
            subprocess.run(
                f"{pineappl} convolute {self.grid} {pdf} --scales 9 --absolute --integrated".split(),
                stdout=fd,
            )
        with open(self.dest / "pineappl.orders", "w") as fd:
            subprocess.run(
                f"{pineappl} orders {self.grid} {pdf} --absolute".split(), stdout=fd
            )
        with open(self.dest / "pineappl.pdf_uncertainty", "w") as fd:
            subprocess.run(
                f"{pineappl} pdf_uncertainty --threads=1 {self.grid} {pdf}".split(),
                stdout=fd,
            )

        return (self.dest / "pineappl.convolute").read_text().splitlines()[2:-2]

    def collect_versions(self):
        versions = {}
        versions["mg5amc_revno"] = (
            subprocess.run(
                "brz revno".split(), cwd=paths.mg5amc, stdout=subprocess.PIPE
            )
            .stdout.decode()
            .strip()
        )
        mg5amc_repo = (
            subprocess.run("brz info".split(), cwd=paths.mg5amc, stdout=subprocess.PIPE)
            .stdout.decode()
            .strip()
        )
        versions["mg5amc_repo"] = re.search(r"\s*parent branch:\s*(.*)", mg5amc_repo)[1]
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


def apply_user_cuts(file_path, user_cuts):
    with open(file_path, "r") as fd:
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

    with open(file_path, "w") as fd:
        fd.writelines(contents)
