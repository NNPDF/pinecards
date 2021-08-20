import time
import pathlib
import subprocess
import json
import re

import click
import rich
import asteval

from . import install, paths, tools

root = pathlib.Path(__file__).absolute().parents[1]


@click.command()
@click.option("--datasets", multiple=True)
@click.option("--pdf", default="NNPDF31_nlo_as_0118_luxqed")
def hadronic(datasets, pdf):
    rich.print("Computing [blue]hadronic[/]...")
    if len(datasets) == 0:
        datasets = tools.select_datasets([p.stem for p in load_datasets()])

    rich.print(datasets)
    install_reqs()
    for name in datasets:
        run_dataset(name, pdf)


def load_datasets():
    proc = root / "nnpdf31_proc"
    hadronic_sets = [p for p in proc.iterdir() if not (p / "observable.yaml").is_file()]
    return hadronic_sets


def install_reqs():
    t0 = time.perf_counter()

    install.update_environ()
    install.mg5amc()
    install.pineappl()

    tools.print_time(t0, "Installation")


def run_dataset(name, pdf):
    t0 = time.perf_counter()

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
    launch_file = dest / "launch.txt"

    # TODO: write a list with variables that should be replaced in the launch file; for the time
    # being we create the file here, but in the future it should be read from the theory database
    variables = json.loads((paths.pkg / "variables.json").read_text())

    # replace the variables with their values
    for name, value in variables.items():
        launch = launch.replace(f"@{name}@", value)

    # perform simple arithmetic on lines containing 'set' and '=' and arithmetic operators
    interpreter = asteval.Interpreter()  # use asteval for safety
    lines = []
    pattern = re.compile(r"(set [\w_]* = )(.*)")
    for line in launch.splitlines():
        m = re.fullmatch(pattern, line)
        if m is not None:
            line = m[1] + str(interpreter.eval(m[2]))
        lines.append(line)
    launch = "\n".join(lines)

    # finally write launch
    launch_file.write_text(launch)

    # launch run
    launch_log = tools.run_subprocess([str(paths.mg5_exe), str(launch_file)], dest=dest)
    (dest / "launch.log").write_text(launch_log)

    tools.print_time(t0, "Grid calculation")
