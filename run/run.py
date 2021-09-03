import time

import click
import rich

from . import install, tools, table
from .external import mg5


@click.command()
@click.argument("dataset")
@click.option("--pdf", default="NNPDF31_nlo_as_0118_luxqed")
def run(dataset, pdf):
    rich.print("Computing [blue]hadronic[/]...")

    rich.print(dataset)
    install_reqs()
    run_dataset(dataset, pdf)


def install_reqs():
    t0 = time.perf_counter()

    install.update_environ()
    install.mg5amc()
    install.pineappl()

    tools.print_time(t0, "Installation")


def run_dataset(name, pdf):
    t0 = time.perf_counter()

    runner = mg5.Mg5(name)

    tools.print_time(t0, "Grid calculation")

    runner.run()
    table.print_table(
        table.parse_pineappl_table(runner.generate_pineappl()),
        runner.results(),
        runner.dest,
    )

    runner.annotate_versions()
    runner.postprocess()
    print(f"Output stored in {runner.dest.name}")
