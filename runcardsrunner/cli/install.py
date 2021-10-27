import click

from .. import install
from ._base import command


@command.group("install")
def subcommand():
    """Install utilities."""


@subcommand.command()
def pineappl():
    """Install pineappl."""
    install.pineappl()


@subcommand.command()
def mg5amc():
    """Install MG5aMC@NLO."""
    install.mg5amc()
