import pathlib
import shutil

import click
import rich

from .. import paths, tools


@click.command("update")
@click.argument("datasets", nargs=-1)
def subcommand(datasets):
    """Update datasets metadata"""
    main(datasets)


def main(datasets):
    for path in datasets:
        path = pathlib.Path(path)
        dataset = path.stem
        # remove doble suffix
        if "pineappl" in dataset:
            dataset = pathlib.Path(dataset).stem

        metadata = paths.runcards / dataset / "metadata.txt"

        entries = {}
        with open(metadata) as fd:
            for line in fd.readlines():
                k, v = line.split("=")
                entries[k] = v

        dest = path.parent / (path.name + ".tmp")
        tools.update_grid_metadata(path, dest, entries)
        compressed = tools.compress(dest)
        dest.unlink()
        shutil.move(compressed, path)

        rich.print(f"'{path}'\n\tgrid metadata updated")
