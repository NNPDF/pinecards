__version__ = "0.0.0"

import click

from .run import run
from .update import update


@click.group()
def cli():
    pass


cli.add_command(run)
cli.add_command(update)
