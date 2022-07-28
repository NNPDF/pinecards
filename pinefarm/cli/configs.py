# -*- coding: utf-8 -*-
import rich

from .. import configs
from ._base import command


@command.command("configs")
def subcommand():
    """Query loaded configurations."""
    main()


def main():
    rich.print(configs.configs)
