# -*- coding: utf-8 -*-
import pathlib
import time

import click
import rich
import yaml

from .. import install, log, table, tools
from ..external import mg5, positivity, yad
from ._base import command


@command.command("run")
@click.argument("dataset")
@click.argument("theory_path", type=click.Path(exists=True))
@click.option("--pdf", default="NNPDF31_nlo_as_0118_luxqed")
def subcommand(dataset, theory_path, pdf):
    """Compute a dataset and compare using a given PDF.

    Parameters
    ----------
    dataset : str
        dataset name
    theory_path : str
        path to theory runcard
    pdf : str
        pdf name

    """
    # read theory card from file
    with open(theory_path) as f:
        theory_card = yaml.safe_load(f)
    main(dataset, theory_card, pdf)


def main(dataset, theory, pdf):
    """Compute a dataset and compare using a given PDF.

    Parameters
    ----------
    dataset : str
        dataset name
    theory : dict
        theory dictionary
    pdf : str
        pdf name

    """
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

    if yad.is_dis(dataset):
        rich.print(f"Computing [red]{dataset}[/]...")
        runner = yad.Yadism(dataset, theory, pdf, timestamp=timestamp)
    elif positivity.is_positivity(dataset):
        rich.print(f"Computing [yellow]{dataset}[/]...")
        runner = positivity.Positivity(dataset, theory, pdf, timestamp=timestamp)
    else:
        rich.print(f"Computing [blue]{dataset}[/]...")
        runner = mg5.Mg5(dataset, theory, pdf, timestamp=timestamp)

    install_reqs(runner, pdf)
    run_dataset(runner)


def install_reqs(runner, pdf):
    """Install requirements.

    Parameters
    ----------
    runner : interface.External
        runner instance
    pdf : str
        pdf name

    """
    t0 = time.perf_counter()

    install.update_environ()
    runner.install()
    install.pineappl()

    # install chosen PDF set
    install.lhapdf_conf(pdf)

    # lhapdf_management determine paths at import time, so it is important to
    # late import it, in particular after environ has been updated by `install.lhlhapdf_conf`
    import lhapdf_management  # pylint: disable=import-error,import-outside-toplevel

    try:
        lhapdf_management.pdf_update()
    # survive even if it's not possible to write 'pdfsets.index'
    except PermissionError:
        pass
    lhapdf_management.pdf_install(pdf)

    tools.print_time(t0, "Installation")


def run_dataset(runner):
    """Execute runner and apply common post process.

    Parameters
    ----------
    runner : interface.External
        runner instance

    """
    t0 = time.perf_counter()

    tools.print_time(t0, "Grid calculation")

    with log.Tee(runner.dest / "errors.log", stdout=False, stderr=True):
        # if output folder specified, do not rerun
        if runner.timestamp is None:
            runner.run()
        # collect results in the output pineappl grid
        runner.generate_pineappl()

        table.print_table(
            table.convolute_grid(
                runner.grid, runner.pdf, integrated=isinstance(runner, mg5.Mg5)
            ),
            runner.results(),
            runner.dest,
        )

        runner.annotate_versions()
        runner.postprocess()

    print(f"Output stored in {runner.dest.name}")
