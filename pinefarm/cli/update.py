# -*- coding: utf-8 -*-
import pathlib
import shutil

import click
import rich

from .. import configs, tools
from ._base import command


@command.command("update")
@click.argument("datasets", nargs=-1)
def subcommand(datasets):
    """Update datasets metadata.

    DATASETS are an arbitrary number of datasets to be updated (if empty, do
    nothing).
    """
    main(datasets)


def main(datasets):
    for path in datasets:
        path = pathlib.Path(path)
        dataset = path.stem
        # remove doble suffix
        if "pineappl" in dataset:
            dataset = pathlib.Path(dataset).stem

        metadata = configs.configs["paths"]["runcards"] / dataset / "metadata.txt"

        with open(metadata) as fd:
            entries = tools.parse_metadata(fd)

        dest = path.parent / (path.name + ".tmp")
        tools.update_grid_metadata(path, dest, entries)
        compressed = tools.compress(dest)
        dest.unlink()
        shutil.move(compressed, path)

        rich.print(f"'{path}'\n\tgrid metadata updated")
