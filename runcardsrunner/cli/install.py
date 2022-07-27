# -*- coding: utf-8 -*-
import click

from .. import install
from ._base import command


@command.group("install")
def subcommand():
    """Install utilities."""
    install.init_prefix()


@subcommand.command()
@click.option("--capi", is_flag=True, default=True, help="install PineAPPL CAPI")
@click.option("--cli", is_flag=True, help="install PineAPPL CLI as well")
def pineappl(capi, cli):
    """Install PineAPPL."""
    install.update_environ()
    install.pineappl(capi=capi, cli=cli)


@subcommand.command()
def mg5amc():
    """Install MG5aMC@NLO."""
    install.update_environ()
    install.mg5amc()


@subcommand.command()
def vrap():
    """Install Hawaiian VRAP."""
    install.update_environ()
    install.hawaiian_vrap()


@subcommand.command()
def lhapdf():
    """Install LHAPDF."""
    install.update_environ()
    install.lhapdf()
