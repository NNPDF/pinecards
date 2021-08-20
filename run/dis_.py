import time

import click
import rich
import yaml
import yadism

from . import install, tools, paths, table, external


@click.command()
@click.option("--datasets", multiple=True)
@click.option("--pdf", default="NNPDF31_nlo_as_0118_luxqed")
def dis(datasets, pdf):
    rich.print("Computing [red]dis[/]...")
    if len(datasets) == 0:
        datasets = tools.select_datasets([p.stem for p in load_datasets()])

    rich.print(datasets)

    install_reqs()

    for name in datasets:
        if tools.avoid_recompute(name):
            continue
        run_dataset(name, pdf)


def load_datasets():
    dis_sets = [
        p for p in paths.runcards.iterdir() if (p / "observable.yaml").is_file()
    ]
    return dis_sets


def install_reqs():
    t0 = time.perf_counter()

    install.update_environ()
    install.pineappl()

    tools.print_time(t0, "Installation")


def run_dataset(name, pdf):
    t0 = time.perf_counter()

    # load runcards
    with open(paths.pkg / "theory.yaml") as t:
        theory = yaml.safe_load(t)
    with open(paths.runcards / name / "observable.yaml") as o:
        obs = yaml.safe_load(o)

    # run yadism
    out = yadism.run_yadism(theory, obs)

    # dump pineappl
    dest = tools.create_folder(name)
    grid_path = dest / f"{name}.pineappl"
    out.dump_pineappl_to_file(str(grid_path), next(iter(obs["observables"].keys())))

    # compress
    cpath = tools.compress(grid_path)
    if cpath.exists():
        grid_path.unlink()
        grid_path = cpath

    table.print_table(
        table.compute_data(grid_path, pdf), external.yadism_results(out, pdf)
    )

    tools.print_time(t0, "Grid calculation")
