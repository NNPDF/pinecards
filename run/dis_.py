import datetime

import click
import rich
import yaml
import yadism

from . import tools, paths


@click.command()
def dis():
    rich.print("Computing [red]dis[/]...")
    datasets = select_datasets()
    rich.print(datasets)
    for name in datasets:
        run_dataset(name)


def load_datasets():
    dis_sets = [
        p for p in paths.runcards.iterdir() if (p / "observable.yaml").is_file()
    ]
    return dis_sets


def select_datasets():
    datasets = [p.stem for p in load_datasets()]
    return tools.select_datasets(datasets)


def run_dataset(name):
    with open(paths.pkg / "theory.yaml") as t:
        theory = yaml.safe_load(t)
    with open(paths.runcards / name / "observable.yaml") as o:
        obs = yaml.safe_load(o)

    out = yadism.run_yadism(theory, obs)
    target = paths.root / (name + datetime.datetime.now().strftime("-%Y%m%d%H%M%S"))
    target.mkdir(exist_ok=True)
    out.dump_pineappl_to_file(
        str(target / f"{name}.pineappl"), next(iter(obs["observables"].keys()))
    )
