# -*- coding: utf-8 -*-
import datetime
import itertools
import subprocess
import time

import lz4.frame
import pineappl
import pygit2
import rich

from . import configs


def create_output_folder(name):
    """Create output folder.

    Parameters
    ----------
    name : str
        dataset name

    Returns
    -------
    pathlib.Path
        path to output folder

    """
    target = configs.configs["paths"]["results"] / (
        name + datetime.datetime.now().strftime("-%Y%m%d%H%M%S")
    )
    target.mkdir(exist_ok=True, parents=True)
    return target


def print_time(t0, what=None):
    """Report completion together with timing to the user.

    Parameters
    ----------
    t0 : int
        start time
    what : str
        subject that is completed

    """
    dt = time.perf_counter() - t0

    if what is None:
        what = ""

    print()
    rich.print(f"[b u]{what}[/] [i green]completed[/]")
    rich.print(f"> took {dt:.2f} s")
    print()


def compress(path):
    """Compress a file into lz4.

    Parameters
    ----------
    path : pathlib.Path
        input path

    Returns
    -------
    pathlib.Path
        path to compressed file

    """
    compressed_path = path.with_suffix(".pineappl.lz4")
    with lz4.frame.open(
        compressed_path, "wb", compression_level=lz4.frame.COMPRESSIONLEVEL_MAX
    ) as fd:
        fd.write(path.read_bytes())

    return compressed_path


def decompress(path):
    """Decompress a file from lz4.

    Parameters
    ----------
    path : pathlib.Path
        path to compressed file

    Returns
    -------
    pathlib.Path
        path to raw file

    """
    with lz4.frame.open(path, "r") as f:
        data = f.read()

    decompressed_path = path.parent / (
        path.stem + ".".join(path.suffix.split(".")[:-1])
    )
    with open(decompressed_path, "wb") as f:
        f.write(data)

    return decompressed_path


def patch(patch, base_dir="."):
    """Apply patch.

    Parameters
    ----------
    patch : str
        patch to apply (text content, not path)
    base_dir : str or pathlib.Path
        path to dir where to apply patch (default: ``.``)

    """
    subprocess.run(
        "patch -p1".split(), cwd=base_dir, input=patch, text=True, check=True
    )


three_points = [0.5, 1.0, 2.0]
"Three points prescription for scale variations."
nine_points = list(itertools.product(three_points, three_points))
"""Nine points prescription for scale variations (as couples, referred to ``(fact,
ren)`` scales)."""


def git_pull(repo, remote_name="origin", branch="master"):
    """Pull a remote repository.

    Parameters
    ----------
    repo : pygit2.Repository
        repository
    remote_name : str
        remote name
    branch : str
        branch to get

    """
    for remote in repo.remotes:
        if remote.name == remote_name:
            remote.fetch()
            remote_master_id = repo.lookup_reference(
                f"refs/remotes/{remote_name}/{branch}"
            ).target
            merge_result, _ = repo.merge_analysis(remote_master_id)
            # Up to date, do nothing
            if merge_result & pygit2.GIT_MERGE_ANALYSIS_UP_TO_DATE:
                return
            # We can just fastforward
            elif merge_result & pygit2.GIT_MERGE_ANALYSIS_FASTFORWARD:
                repo.checkout_tree(repo.get(remote_master_id))
                master_ref = repo.lookup_reference(f"refs/heads/{branch}")
                master_ref.set_target(remote_master_id)
                repo.head.set_target(remote_master_id)
            else:
                raise AssertionError(f"Impossible to pull git repo '{repo.path}'")


def update_grid_metadata(input_file, output_file, entries=None, entries_from_file=None):
    """Set metadata on a pineappl grid stored in a file, and save in a new one.

    Parameters
    ----------
    input_file : path-like
        file storing input grid
    output_file : path-like
        file to store the result
    entries : dict
        mapping of key-value to store in the grid
    entries_from_file : dict
        mapping of key-value pairs, whose value are file paths of which
        storing the content

    """
    grid = pineappl.grid.Grid.read(str(input_file))
    set_grid_metadata(grid, entries, entries_from_file)

    grid.write(str(output_file))


def set_grid_metadata(grid, entries=None, entries_from_file=None):
    """Set metadata on a pineappl grid (in-place operation)

    Parameters
    ----------
    input_grid : pineappl.grid.Grid
        input grid on which to set metadata
    entries : dict
        mapping of key-value to store in the grid
    entries_from_file : dict
        mapping of key-value pairs, whose value are file paths of which
        storing the content

    """
    if entries is None:
        entries = {}
    if entries_from_file is None:
        entries_from_file = {}

    for k, v in entries.items():
        grid.set_key_value(k, v)

    for k, v in entries_from_file.items():
        with open(v) as fd:
            grid.set_key_value(k, fd.read())


def common_substring(s1, s2, *sn):
    """Return the longest common part of two iterables, starting from the begininng

    Parameters
    ----------
    s1 : Sequence
        first sequence to compare
    s2 : Sequence
        second sequence to compare
    *sn : Sequence
        any further sequence to compare

    Returns
    -------
    Sequence
        longest common subsequence

    Examples
    --------
    >>> common_substring("test test", "test toast")
    "test t"
    >>> common_substring("test test", "test test test")
    "test test"
    >>> common_substring("test test", "")
    ""
    >>> common_substring("test test", "test test test", "test")
    "test"
    >>> common_substring("test test", "test test test", "test toast")
    "test t"

    """
    try:
        # since it is common to all, we can take the matching chunk from the first
        return s1[
            # stop at first non-matching element
            : next(
                n
                # iterate over all elements
                for n, (c1, c2, *cn) in enumerate(zip(s1, s2, *sn))
                # if they are all equal keep going
                if len({c1, c2, *cn}) > 1
            )
        ]
    except StopIteration:
        # if they match up to the end of one of the inputs, return the shortest
        ss = (s1, s2, *sn)
        # sort by length and take the first
        shortest = min(enumerate(len(s) for s in ss), key=lambda el: el[1])[0]
        return ss[shortest]


def parse_metadata(file):
    """Parse metadata file.

    Parameters
    ----------
    file : io.TextIOBase
        the file to read

    Returns
    -------
    dict
        the metadata entries

    """
    entries = {}
    for line in file.readlines():
        if line[-1] == "\n":
            line = line[:-1]

        k, v = line.split("=")
        entries[k] = v

    return entries
