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
    if tools.avoid_recompute(dataset):
        return

    if tools.is_dis(dataset):
        rich.print(f"Computing [red]{dataset}[/]...")
        runner = yad.Yadism(dataset, pdf)
    else:
        rich.print(f"Computing [blue]{dataset}[/]...")
        runner = mg5.Mg5(dataset, pdf)

    install_reqs(runner, pdf)
    run_dataset(runner, dataset, pdf)


def install_reqs(runner, pdf):
    # lhapdf_management determine paths at import time, so it is important to
    # late import it, in particular after `.path` module has been imported
    import lhapdf_management

    t0 = time.perf_counter()

    install.update_environ()
    runner.install()
    install.pineappl()
    lhapdf_management.pdf_install(pdf)

    tools.print_time(t0, "Installation")


def run_dataset(runner, name, pdf):
    t0 = time.perf_counter()

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
