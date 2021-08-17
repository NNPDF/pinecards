import click
import rich
import yaml
import yadism
import lhapdf

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
        pdf, lambda muR: lhapdf.mkAlphaS(pdf_name).alphasQ(muR), lambda muR: 0, 1.0, 1.0
    )
    pdf_out = next(iter(pdf_out.tables.values()))
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

    table.print_table(table.compute_data(grid_path, pdf), yadism_results(out, pdf))

    # compress
    tools.compress(grid_path)
    if (grid_path.parent / (grid_path.name + ".lz4")).exists():
        grid_path.unlink()
