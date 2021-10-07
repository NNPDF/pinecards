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
    """Compute a dataset and compare using a given PDF."""

    dataset = pathlib.Path(dataset).name
    timestamp = None

    if "-" in dataset:
        try:
            dataset, timestamp = dataset.split("-")
        except:
            raise ValueError(
                f"'{dataset}' not valid. '-' is only allowed once,"
                " to separate dataset name from timestamp."
            )

    rich.print(dataset)

    if tools.is_dis(dataset):
        rich.print(f"Computing [red]{dataset}[/]...")
        runner = yad.Yadism(dataset, pdf, timestamp=timestamp)
    else:
        rich.print(f"Computing [blue]{dataset}[/]...")
        runner = mg5.Mg5(dataset, pdf, timestamp=timestamp)

    install_reqs(runner, pdf)
    run_dataset(runner)


def install_reqs(runner, pdf):
    """
    Execute runner and apply common post process.

    Parameters
    ----------
        runner : interface.External
            runner instance
        pdf : str
            pdf name
    """
    # lhapdf_management determine paths at import time, so it is important to
    # late import it, in particular after `.path` module has been imported
    import lhapdf_management  # pylint: disable=import-outside-toplevel

    t0 = time.perf_counter()

    install.update_environ()
    runner.install()
    install.pineappl()
    lhapdf_management.pdf_update()
    lhapdf_management.pdf_install(pdf)

    tools.print_time(t0, "Installation")


def run_dataset(runner):
    """
    Execute runner and apply common post process.

    Parameters
    ----------
        runner : interface.External
            runner instance
    """
    t0 = time.perf_counter()

    tools.print_time(t0, "Grid calculation")

    # if output folder specified, do not rerun
    if runner.timestamp is None:
        runner.run()
    table.print_table(
        table.parse_pineappl_table(runner.generate_pineappl()),
        runner.results(),
        runner.dest,
    )

    runner.annotate_versions()
    runner.postprocess()
    print(f"Output stored in {runner.dest.name}")
