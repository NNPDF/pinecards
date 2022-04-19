# -*- coding: utf-8 -*-
import itertools
import pathlib
import re
import shutil

import click
import pineappl
import rich

from .. import tools
from ._base import command


@command.command("merge")
@click.argument("grids", nargs=-1)
def subcommand(grids):
    """Merge multiple PineAPPL grids into a single one."""

    main(grids)


def main(grids):
    if len(grids) < 2:
        raise ValueError("At least 2 grids needed for a merge.")

    grid_paths = [pathlib.Path(grid) for grid in grids]
    grids = [pineappl.grid.Grid.read(str(grid)) for grid in grids]

    common = tools.common_substring(*(grid.name for grid in grid_paths)).strip("_")
    mgrid_path = pathlib.Path(common).with_suffix(".pineappl")
    rich.print(f"Merging into -> '{mgrid_path}'")

    # merge all grids in a single one
    # read the first one from file
    rich.print(f"Init to '{grid_paths[0]}'")
    mgrid = grids[0]
    # subsequently merge all the others (disk -> memory)
    for path in grid_paths[1:]:
        rich.print(f"Merging '{path}'...")
        mgrid.merge_from_file(str(path))
    mgrid.write(str(mgrid_path))

    # get all keys, possibly ones that are exclusive to a single grid
    keys = set(itertools.chain(*(grid.key_values().keys() for grid in grids)))

    # concatenate results
    tmpresults = []
    data_row = re.compile(r"\d.*")
    empty_row = re.compile(
        r"\d+ +0.000000e+00  0.000000e+00   0.000   0.000   0.0000 0.0000   0.0000"
    )

    # extract the header from the first grid
    for line in mgrid.key_values().get("results").splitlines():
        results_header_rows = []
        if not data_row.fullmatch(line):
            results_header_rows.append(line)
            tmpresults.append("\n".join(results_header_rows))

    # extract the results from each grid
    for grid in grids:
        results_rows = []
        for line in grid.key_values().get("results").splitlines():
            if data_row.fullmatch(line) and not empty_row.fullmatch(line):
                results_rows.append(line)
        tmpresults.append("\n".join(results_rows))

    # set the results metadata in the new grid
    mgridtmp = mgrid_path.parent / (mgrid_path.name + ".tmp")
    tools.update_grid_metadata(
        mgrid_path, mgridtmp, entries={"results": "\n".join(tmpresults)}
    )
    shutil.move(str(mgridtmp), mgrid_path)

    mkeys = mgrid.key_values()
    for key in keys:
        if key == "results":
            continue

        mvalue = mkeys.get(key)
        for grid in grids:
            if mvalue != grid.key_values().get(key):
                # TODO: what do we do in this case?
                rich.print(f"'{key}' differs [gray]for '{grid}'[/]")

    cpath = tools.compress(mgrid_path)
    mgrid_path.unlink()
    rich.print(f"Grid merged and compressed, stored in '{cpath}'.")
