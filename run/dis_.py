import pathlib

import click
import rich

from . import tools

root = pathlib.Path(__file__).absolute().parents[1]


@click.command()
def dis():
    rich.print("Computing [red]dis[/]...")
    rich.print(select_datasets())


def load_datasets():
    proc = root / "nnpdf31_proc"
    dis_sets = [p for p in proc.iterdir() if (p / "observable.yaml").is_file()]
    return dis_sets


def select_datasets():
    datasets = [p.stem for p in load_datasets()]
    return tools.select_datasets(datasets)
