import datetime
import itertools

# import re
# import readline
import subprocess
import time

import InquirerPy
import lz4.frame
import pineappl
import pygit2
import rich

from . import paths

# from difflib import SequenceMatcher


# def similar(a, b):
#     """
#     Compare strings

#     .. todo::

#         use to suggest dataset in prompt
#     """
#     return SequenceMatcher(None, a, b).ratio()

# COMMANDS = ["extra", "extension", "stuff", "errors", "email", "foobar", "foo"]


# re_space = re.compile(".*\s+$", re.M)


def is_dis(name):
    """
    Is this a DIS dataset, i.e. is yadism needed to run?

    The decision is based on the existance of the `observable.yaml` file.

    Parameters
    ----------
        name : str
            dataset name
    """
    return (paths.runcards / name / "observable.yaml").exists()


# class Completer:
#     def __init__(self, tokens):
#         self.tokens = tokens

#     def complete(self, text, state):
#         "Generic readline completion entry point."
#         buffer = readline.get_line_buffer()
#         line = readline.get_line_buffer().split()
#         # show all commands
#         if not line:
#             return [c + " " for c in self.tokens][state]
#         # account for last argument ending in a space
#         if re_space.match(buffer):
#             line.append("")
#         # resolve command to the implementation function
#         token = line[-1].strip()
#         results = [c + " " for c in self.tokens if c.startswith(token)] + [None]
#         return results[state]


# def select_datasets(datasets_list):
#     comp = Completer(datasets_list)
#     # we want to treat '/' as part of a word, so override the delimiters
#     readline.set_completer_delims(" \t\n;")
#     readline.parse_and_bind("tab: complete")
#     readline.set_completer(comp.complete)
#     rich.print("[i magenta]Enter dataset name:")
#     ans = input("> ")
#     readline.set_completer()
#     return ans.split()


def avoid_recompute(name):
    """
    Ask the user whether he really wants to recompute the dataset.

    Parameters
    ----------
        name : str
            dataset name

    Returns
    -------
        bool :
            do NOT recompute?
    """
    for p in paths.root.iterdir():
        if p.is_dir() and name in p.name:
            rich.print(
                f"[i grey50]dataset already in '[/][yellow]{p.name}[i grey50]'[/]"
            )
            questions = [
                {
                    "type": "confirm",
                    "name": "recompute",
                    "message": f"Do you want to recompute '{name}'?\n",
                }
            ]
            answers = InquirerPy.prompt(questions)
            return not answers["recompute"]
    return False


def create_folder(name):
    """
    Create output folder.

    Parameters
    ----------
        name : str
            dataset name

    Returns
    -------
        pathlib.Path :
            path to output folder
    """
    target = paths.root / (name + datetime.datetime.now().strftime("-%Y%m%d%H%M%S"))
    target.mkdir(exist_ok=True)
    return target


def print_time(t0, what=None):
    """
    Report completion together with timing to the user.

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
    """
    Compress a file into lz4.

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
    """
    Decompress a file from lz4.

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


three_points = [0.5, 1.0, 2.0]
nine_points = itertools.product(three_points, three_points)


def git_pull(repo, remote_name="origin", branch="master"):
    """
    Pull a remote repository.

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


def run_subprocess(*args, dest):
    """
    Wrapper to :class:`subprocess.Popen` to print the output to screen and capture it.

    Returns
    -------
        str :
            output
    """
    p = subprocess.Popen(
        *args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=dest
    )
    output = []

    while True:
        # returns None while subprocess is running
        retcode = p.poll()
        line = p.stdout.readline().decode()[:-1]
        if retcode is not None:
            break
        print(line)
        output.append(line)

    return "\n".join(output)


def set_grid_metadata(input_file, output_file, entries=None, entries_from_file=None):
    """
    Set metadata on a pineappl grid stored in a file, and save in a new one.

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
    if entries is None:
        entries = {}
    if entries_from_file is None:
        entries_from_file = {}
    grid = pineappl.grid.Grid.read(str(input_file))

    for k, v in entries.items():
        grid.set_key_value(k, v)

    for k, v in entries_from_file.items():
        with open(v) as fd:
            grid.set_key_value(k, fd.read())

    grid.write(str(output_file))


def common_substring(s1, s2, *sn):
    """
    Return the longest common part of two iterables, starting from the begininng

    Parameters
    ----------
        s1 : sequence
        s2 : sequence

    Returns
    -------
        sequence
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
                if len(set([c1, c2, *cn])) > 1
            )
        ]
    except StopIteration:
        # if they match up to the end of one of the inputs, return the shortest
        ss = (s1, s2, *sn)
        # sort by length and take the first
        shortest = min(enumerate(len(s) for s in ss), key=lambda el: el[1])[0]
        return ss[shortest]
