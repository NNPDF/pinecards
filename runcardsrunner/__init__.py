__version__ = "0.0.0"

import click

from . import cli


@click.group()
def command():
    pass


command.add_command(cli.merge.subcommand)
command.add_command(cli.run.subcommand)
command.add_command(cli.update.subcommand)
