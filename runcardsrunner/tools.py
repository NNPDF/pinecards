import datetime
import itertools

# import re
# import readline
import subprocess
import time

import InquirerPy
import lz4.frame
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
    with open(path, "rb") as f:
        data = f.read()

    compressed_path = path.parent / (path.name + ".lz4")
    with lz4.frame.open(compressed_path, "wb") as f:
        f.write(data)

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
