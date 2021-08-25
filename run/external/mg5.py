import subprocess
import json
import re
import shutil

from .. import paths, tools
from . import interface


class Mg5(interface.External):
    pass


def run_mc(name):
    source = paths.runcards / name
    dest = tools.create_folder(name)
    mg5_dir = dest / name

    # copy the output file to the directory and replace the variables
    output = (source / "output.txt").read_text().replace("@OUTPUT@", name)
    output_file = dest / "output.txt"
    output_file.write_text(output)

    # create output folder
    output_log = tools.run_subprocess([str(paths.mg5_exe), str(output_file)], dest=dest)
    (dest / "output.log").write_text(output_log)

    # copy patches if there are any; use xargs to properly signal failures
    for p in source.iterdir():
        if p.suffix == ".patch":
            subprocess.run(
                "patch -p1".split(), input=p.read_text(), text=True, cwd=mg5_dir
            )

    # enforce proper analysis
    # - copy analysis.f
    analysis = (paths.runcards / name / "analysis.f").read_text()
    (mg5_dir / "FixedOrderAnalysis" / f"{name}.f").write_text(analysis)
    # - update analysis card
    analysis_card = mg5_dir / "Cards" / "FO_analyse_card.dat"
    analysis_card.write_text(
        analysis_card.read_text().replace("analysis_HwU_template", name)
    )

    # copy the launch file to the directory and replace the variables
    launch = (source / "launch.txt").read_text().replace("@OUTPUT@", name)

    # TODO: write a list with variables that should be replaced in the launch file; for the time
    # being we create the file here, but in the future it should be read from the theory database
    variables = json.loads((paths.pkg / "variables.json").read_text())

    # replace the variables with their values
    for name, value in variables.items():
        launch = launch.replace(f"@{name}@", value)

    # perform simple arithmetic on lines containing 'set' and '=' and arithmetic operators
    #  interpreter = asteval.Interpreter()  # use asteval for safety
    #  lines = []
    #  pattern = re.compile(r"(set [\w_]* = )(.*)")
    #  for line in launch.splitlines():
    #  m = re.fullmatch(pattern, line)
    #  if m is not None:
    #  line = m[1] + str(interpreter.eval(m[2]))
    #  lines.append(line)
    #  launch = "\n".join(lines)

    # finally write launch
    launch_file = dest / "launch.txt"
    launch_file.write_text(launch)

    user_cuts_pattern = re.compile(
        r"^#user_defined_cut set (\w+)\s+=\s+([+-]?\d+(?:\.\d+)?|True|False)$"
    )
    user_cuts = []
    for line in launch.splitlines():
        m = re.fullmatch(user_cuts_pattern, line)
        if m is not None:
            user_cuts.append((m[1], m[2]))

    apply_user_cuts(mg5_dir / "SubProcesses" / "cuts.f", user_cuts)

    # launch run
    launch_log = tools.run_subprocess([str(paths.mg5_exe), str(launch_file)], dest=dest)
    (dest / "launch.log").write_text(launch_log)

    return dest


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


def merge(name, dest):
    source = paths.runcards / name
    mg5_dir = dest / name
    grid = dest / f"{name}.pineappl"
    gridtmp = dest / f"{name}.pineappl.tmp"

    # merge the final bins
    mg5_grids = " ".join(
        str(p) for p in dest.glob("Events/run_01*/amcblast_obs_*.pineappl")
    )
    subprocess.run(f"pineappl merge {grid} {mg5_grids}".split())

    # optimize the grids
    subprocess.run(f"pineappl optimize {grid} {gridtmp}".split())
    shutil.move(gridtmp, grid)

    # add metadata
    metadata = source / "metadata.txt"
    runcard = next(iter(mg5_dir.glob("Events/run_01*/run_01*_tag_1_banner.txt")))
    entries = ""
    if metadata.exists():
        for line in metadata.read_text().splitlines():
            k, v = line.split("=")
            entries += f"--entry {k} '{v}' "
    subprocess.run(
        f"pineappl set {grid} {gridtmp}".split()
        + entries.split()
        + f"--entry_from_file runcard {runcard}".split()
    )
    shutil.move(gridtmp, grid)

    # find out which PDF set was used to generate the predictions
    pdf = re.search(r"set lhaid (\d+)", (dest / "launch.txt").read_text())[1]

    # (re-)produce predictions
    with open(dest / "pineappl.convolute", "w") as fd:
        subprocess.run(
            f"pineappl convolute {grid} {pdf} --scales 9 --absolute --integrated".split(),
            stdout=fd,
        )
    with open(dest / "pineappl.orders", "w") as fd:
        subprocess.run(f"pineappl orders {grid} {pdf} --absolute".split(), stdout=fd)
    with open(dest / "pineappl.pdf_uncertainty", "w") as fd:
        subprocess.run(
            f"pineappl pdf_uncertainty --threads=1 {grid} {pdf}".split(), stdout=fd
        )
