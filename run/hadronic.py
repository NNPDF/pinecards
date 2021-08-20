import pathlib

import click
import rich

from . import install, tools

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
    install.update_environ()
    install.mg5amc()
    install.pineappl()


def run_dataset(name, pdf):
    pass
