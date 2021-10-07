__version__ = "0.0.0"

import click

from .merge import merge
from .run import subcommand as sc_run
from .update import update


@click.group()
def cli():
    pass


cli.add_command(merge)
cli.add_command(sc_run)
cli.add_command(update)
