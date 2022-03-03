# -*- coding: utf-8 -*-
import pathlib

import click
import rich

from .. import configs, tools
from ..external.yad import is_dis
from ._base import command


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
    info = {}

    for dataset in datasets:
        dataset = pathlib.Path(dataset).name
        info[dataset] = {}

        path = configs.configs["paths"]["runcards"] / dataset

        if path.is_dir():
            info[dataset]["path"] = str(path.absolute())

        if metadata:
            metadata = path / "metadata.txt"
            with open(metadata, encoding="utf-8") as fd:
                info[dataset]["metadata"] = tools.parse_metadata(fd)

        if kind:
            if is_dis(dataset):
                info[dataset]["kind"] = "dis"
            else:
                info[dataset]["kind"] = "hadronic"

    rich.print_json(data=info)
