import pathlib
import time

import click
import rich

from . import install, table, tools
from .external import mg5, yad


@click.command()
@click.argument("dataset")
@click.option("--pdf", default="NNPDF31_nlo_as_0118_luxqed")
def run(dataset, pdf):
    dataset = pathlib.Path(dataset).name

    rich.print(dataset)

    if tools.is_dis(dataset):
        rich.print(f"Computing [red]{dataset}[/]...")
        runner = yad.Yadism(dataset, pdf)
    else:
        rich.print(f"Computing [blue]{dataset}[/]...")
        runner = mg5.Mg5(dataset, pdf)

    install_reqs(runner)
    if tools.avoid_recompute(dataset):
        return
    run_dataset(runner, dataset, pdf)


def install_reqs(runner):
    t0 = time.perf_counter()

    install.update_environ()
    runner.install()
    install.pineappl()

    tools.print_time(t0, "Installation")


def run_dataset(runner, name, pdf):
    t0 = time.perf_counter()

    tools.print_time(t0, "Grid calculation")

    runner.run()
    # we need results first because this may install the PDF set
    results = runner.results()
    table.print_table(
        table.parse_pineappl_table(runner.generate_pineappl()),
        results,
        runner.dest,
    )

    runner.annotate_versions()
    runner.postprocess()
    print(f"Output stored in {runner.dest.name}")
