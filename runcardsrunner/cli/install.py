# -*- coding: utf-8 -*-
import click

from .. import install
from ._base import command


@command.group("install")
def subcommand():
    """Install utilities."""


@subcommand.command()
@click.option("--cli", is_flag=True, help="install PineAPPL CLI as well")
def pineappl(cli):
    """Install pineappl."""
    install.update_environ()
    install.pineappl(cli)


@subcommand.command()
def mg5amc():
    """Install MG5aMC@NLO."""
    install.update_environ()
    install.mg5amc()
