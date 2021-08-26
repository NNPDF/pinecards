import time

import click
import rich

from . import install, paths, tools, table
from .external import mg5


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
    hadronic_sets = [
        p for p in paths.runcards.iterdir() if not (p / "observable.yaml").is_file()
    ]
    return hadronic_sets


def install_reqs():
    t0 = time.perf_counter()

    install.update_environ()
    install.mg5amc()
    install.pineappl()

    tools.print_time(t0, "Installation")


def run_dataset(name, pdf):
    t0 = time.perf_counter()

    dest = mg5.run_mc(name)
    grid, pdf = mg5.merge(name, dest)

    tools.print_time(t0, "Grid calculation")

    table.print_table(table.compute_data(grid, pdf), mg5.results(dest, dest / name))

    mg5.annotate_versions(name, dest)
    mg5.postrun(name, dest)
