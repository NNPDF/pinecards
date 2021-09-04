import time

import click
import rich

from . import install, tools, paths, table
from .external import yad


@click.command()
@click.option("--datasets", multiple=True)
@click.option("--pdf", default="NNPDF31_nlo_as_0118_luxqed")
def dis(datasets, pdf):


def install_reqs():
    t0 = time.perf_counter()

    install.update_environ()
    install.pineappl()

    tools.print_time(t0, "Installation")
