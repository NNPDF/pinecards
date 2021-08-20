import datetime
import itertools
from difflib import SequenceMatcher

import rich
import lz4.frame
import pygit2
import PyInquirer

from . import paths


def similar(a, b):
    """
    Compare strings

    .. todo::

        use to suggest dataset in prompt
    """
    return SequenceMatcher(None, a, b).ratio()


import re
import readline

COMMANDS = ["extra", "extension", "stuff", "errors", "email", "foobar", "foo"]


re_space = re.compile(".*\s+$", re.M)


class Completer:
    def __init__(self, tokens):
        self.tokens = tokens

    def complete(self, text, state):
        "Generic readline completion entry point."
        buffer = readline.get_line_buffer()
        line = readline.get_line_buffer().split()
        # show all commands
        if not line:
            return [c + " " for c in self.tokens][state]
        # account for last argument ending in a space
        if re_space.match(buffer):
            line.append("")
        # resolve command to the implementation function
        token = line[-1].strip()
        results = [c + " " for c in self.tokens if c.startswith(token)] + [None]
        return results[state]


def select_datasets(datasets_list):
    comp = Completer(datasets_list)
    # we want to treat '/' as part of a word, so override the delimiters
    readline.set_completer_delims(" \t\n;")
    readline.parse_and_bind("tab: complete")
    readline.set_completer(comp.complete)
    rich.print("[i magenta]Enter dataset name:")
    ans = input("> ")
    readline.set_completer()
    return ans.split()


def avoid_recompute(name):
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
            answers = PyInquirer.prompt(questions)
            return not answers["recompute"]
    return False


def create_folder(name):
    target = paths.root / (name + datetime.datetime.now().strftime("-%Y%m%d%H%M%S"))
    target.mkdir(exist_ok=True)
    return target / f"{name}.pineappl"


def compress(path):
    with open(path, "rb") as f:
        data = f.read()

    compressed_path = path.parent / (path.name + ".lz4")
    with lz4.frame.open(compressed_path, "wb") as f:
        f.write(data)

    return compressed_path


def decompress(path):
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
