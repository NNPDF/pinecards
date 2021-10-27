__version__ = "0.0.0"

import click

from .mergem import subcommand as sc_merge
from .runm import subcommand as sc_run
from .updatem import subcommand as sc_update


@click.group()
def cli():
    pass


cli.add_command(sc_merge)
cli.add_command(sc_run)
cli.add_command(sc_update)
