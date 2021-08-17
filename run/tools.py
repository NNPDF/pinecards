import datetime
from difflib import SequenceMatcher

import rich
import lz4.frame

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
