import itertools
import pathlib
import re

import click
import pineappl
import rich

from .. import tools


@click.command("merge")
@click.argument("grids", nargs=-1)
def subcommand(grids):
    """Merge multiple PineAPPL grids into a single one"""

    main(grids)


def main(grids):
    if len(grids) < 2:
        raise ValueError("At least 2 grids needed for a merge.")

    grids = [pathlib.Path(grid) for grid in grids]
    common = tools.common_substring(*(grid.name for grid in grids)).strip("_")

    raise NotImplementedError("Missing 'merge()' in pineappl_py")
    # merge grids
    mgrid = pathlib.Path(common).with_suffix(".pineappl")
    pineappl.grid.merge(mgrid, *grids)

    raise NotImplementedError("Missing 'Grid.keys()' in pineappl_py")
    # get all keys, possibly ones that are exclusive to a single grid
    keys = set(itertools.chain(grid.keys() for grid in grids))

    # concatenate results
    tmpresults = []
    data_row = re.compile(" \d.*")
    empty_row = " 0.000000e+00  0.000000e+00   0.000   0.000   0.0000 0.0000   0.0000"

    # extract the header from the first grid
    for line in grids[0].get("results").splitlines():
        results_header_rows = []
        if not data_row.fullmatch(line):
            results_header_rows.append(line)
        tmpresults.append("\n".join(results_header_rows))

    # extract the results from each grid
    for grid in grids:
        results_rows = []
        for line in grid.get("results").splitlines():
            if data_row.fullmatch(line) and line != empty_row:
                results_rows.append(line)
        tmpresults.append("\n".join(results_rows))

    # set the results metadata in the new grid
    mgridtmp = mgrid.parent / (mgrid.name + ".tmp")
    tools.update_grid_metadata(
        mgrid, mgridtmp, entries_from_file={"results": "\n".join(tmpresults)}
    )
    shutil.move(mgridtmp, mgrid)

    for key in keys:
        if key == "results":
            continue

        mvalue = mgrid.get(key)
        for grid in grids:
            if mvalue != grid.get(key):
                # TODO: what do we do in this case?
                rich.print(f"'{key}' differs [gray]for '{grid}'[/]")

    tools.compress(mgrid)
    mgrid.unlink()
