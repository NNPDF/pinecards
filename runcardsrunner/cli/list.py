import click

from .. import paths, tools
from ._base import command


@command.group("list")
def subcommand():
    """List available resources."""


@subcommand.command()
def runcards():
    """List available runcards."""
    runcards_main()


@subcommand.command()
def theories():
    """List available theories."""
    theories_main()


def runcards_main():
    print("ciao")


def theories_main():
    print("come")
