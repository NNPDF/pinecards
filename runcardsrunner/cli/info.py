# -*- coding: utf-8 -*-
import pathlib

import click
import rich

from .. import configs, info, tools
from ._base import command


# Meta-function to generate the group `info`
@command.group("info")
def subcommand():
    """Retrieve information about managed objects."""


@subcommand.command("runcards")
@click.argument("datasets", nargs=-1)
@click.option("-m", "--metadata", is_flag=True, help="Retrieve metadata.")
@click.option("-k", "--kind", is_flag=True, help="Retrieve datasets kind.")
def runcards(datasets, metadata, kind):
    """Inspect runcards.

    Obtain information about DATASET runcard.
    """
    # collect requested info in a dictionary
    infod = {}

    for dataset in datasets:
        dataset = pathlib.Path(dataset).name
        infod[dataset] = {}
        datainfo = infod[dataset]

        path = configs.configs["paths"]["runcards"] / dataset

        if path.is_dir():
            datainfo["path"] = str(path.absolute())

        if metadata:
            metadata = path / "metadata.txt"
            with open(metadata, encoding="utf-8") as fd:
                datainfo["metadata"] = tools.parse_metadata(fd)

        if kind:
            datainfo["kind"] = info.label(dataset).kind.name

    rich.print_json(data=infod)
