import click
import rich
import yaml
import yadism
import lhapdf
from functools import reduce

import pandas as pd

from . import tools, paths, table


@click.command()
@click.option("--datasets", multiple=True)
@click.option("--pdf", default="NNPDF31_nlo_as_0118_luxqed")
def dis(datasets, pdf):
    rich.print("Computing [red]dis[/]...")
    if len(datasets) == 0:
        datasets = select_datasets()

    rich.print(datasets)
    for name in datasets:
        run_dataset(name, pdf)


def load_datasets():
    dis_sets = [
        p for p in paths.runcards.iterdir() if (p / "observable.yaml").is_file()
    ]
    return dis_sets


def select_datasets():
    datasets = [p.stem for p in load_datasets()]
    return tools.select_datasets(datasets)


def yadism_results(out, pdf_name):
    pdf = lhapdf.mkPDF(pdf_name)
    pdf_out = out.apply_pdf_alphas_alphaqed_xir_xif(
        pdf,
        lambda muR: lhapdf.mkAlphaS(pdf_name).alphasQ(muR),
        lambda _muR: 0,
        1.0,
        1.0,
    )
    pdf_out = next(iter(pdf_out.tables.values()))

    sv_pdf_out = []
    for xiR, xiF in tools.nine_points:
        sv_point = out.apply_pdf_alphas_alphaqed_xir_xif(
            pdf,
            lambda muR: lhapdf.mkAlphaS(pdf_name).alphasQ(muR),
            lambda _muR: 0.0,
            xiR,
            xiF,
        )
        df = (
            next(iter(sv_point.tables.values()))
            .rename({"result": (xiR, xiF)}, axis=1)
            .drop("error", axis=1)
        )
        sv_pdf_out.append(df)

    sv_pdf_merged = reduce(
        lambda left, right: pd.merge(left, right, on=["x", "Q2"], how="outer"),
        sv_pdf_out,
    )
    svdf = sv_pdf_merged[
        list(filter(lambda name: isinstance(name, tuple), sv_pdf_merged.columns))
    ]
    pdf_out["sv_max"] = svdf.max(axis=1)
    pdf_out["sv_min"] = svdf.min(axis=1)

    return pdf_out


def run_dataset(name, pdf):
    # load runcards
    with open(paths.pkg / "theory.yaml") as t:
        theory = yaml.safe_load(t)
    with open(paths.runcards / name / "observable.yaml") as o:
        obs = yaml.safe_load(o)

    # run yadism
    out = yadism.run_yadism(theory, obs)

    # dump pineappl
    grid_path = tools.create_folder(name)
    out.dump_pineappl_to_file(str(grid_path), next(iter(obs["observables"].keys())))

    # compress
    cpath = tools.compress(grid_path)
    if cpath.exists():
        grid_path.unlink()
        grid_path = cpath

    table.print_table(table.compute_data(grid_path, pdf), yadism_results(out, pdf))
