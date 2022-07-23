# -*- coding: utf-8 -*-
import os
import pathlib
import shutil
import tempfile
import warnings
from typing import Optional

import appdirs
import tomli

NAME = "runcardsrunner.toml"
"""Name of the config while (wherever it is placed)"""

configs = {}
"Holds loaded configurations"


def detect(path: Optional[os.PathLike] = None) -> pathlib.Path:
    """Detect configuration files.

    Parameters
    ----------
    path: os.PathLike or None
        optional explicit path to file to be used as configs (default: `None`)

    Returns
    -------
    pathlib.Path
        configuration file path

    Raises
    ------
    FileNotFoundError
        in case no valid configuration file is found

    """
    paths = []

    if path is not None:
        path = pathlib.Path(path)
        paths.append(path)

    paths.append(pathlib.Path.cwd())
    paths.append(pathlib.Path.home())
    paths.append(pathlib.Path(appdirs.user_config_dir()))

    for p in paths:
        configs_file = p / NAME if p.is_dir() else p

        if configs_file.is_file():
            return configs_file

        if p == path:
            warnings.warn("Configuration path specified is not valid.")

    raise FileNotFoundError("No configurations file detected.")


def load(path: Optional[os.PathLike] = None) -> dict:
    """Load configuration file.

    If no path is explicitly passed, a minimal configuration is used instead
    (just setting root folder to the current one).

    Parameters
    ----------
    path: os.PathLike or None
        the path to the configuration file (default: `None`)

    Returns
    -------
    dict
        loaded configurations

    """
    if path is None:
        warnings.warn("Using default minimal configuration ('root = $PWD').")
        return {"paths": {"root": pathlib.Path.cwd()}}

    with open(path, "rb") as fd:
        loaded = tomli.load(fd)

    if "root" not in loaded["paths"]:
        loaded["paths"]["root"] = pathlib.Path(path).parent

    return loaded


def nestupdate(base: dict, update: dict):
    """Merge nested dictionaries.

    Pay attention, `base` will be mutated in place.
    So the second one will overwrite the first.

    Note
    ----
    Modifying in place avoids a lot of copies.
    But not being performance intensive, it would be possible to obtain a non
    in-place alternative just adding a first line::

        base = copy.deepcopy(base)

    but it would be called at every recursion (the lots of copies above).
    A simpler alternative is just to copy before calling, if needed::

        mycopy = copy.deepcopy(mydict)
        nestupdate(mycopy, update)

    that will make a single copy.

    Parameters
    ----------
    base: dict
        dictionary to be updated
    update: dict
        dictionary containing update

    """

    def newval(old, val):
        """Build new value.

        If one of the two is not a dictionary, simply use update value `val`.
        If they are both dictionaries, start recursion.

        """
        if not isinstance(val, dict) or not isinstance(old, dict):
            return val

        nestupdate(old, val)
        return old

    # add every value of update into base
    for key, val in update.items():
        try:
            # attempt to merge update value with old one
            old = base[key]
            base[key] = newval(old, val)
        except KeyError:
            # if value did not exist, simply add it
            base[key] = val


def defaults(base_configs):
    """Provide additional defaults.

    Note
    ----
    The general rule is to never replace user provided input.

    """
    configs = {}

    configs["paths"] = add_paths(base_configs["paths"]["root"])
    configs["commands"] = add_commands(configs["paths"])

    return configs


def add_paths(root):
    paths = {}

    paths["root"] = root
    paths["runcards"] = root / "runcards"
    paths["theories"] = root / "theories"
    paths["prefix"] = root / ".prefix"
    paths["results"] = root / "results"

    paths["rust_init"] = pathlib.Path(tempfile.mktemp())

    prefix = paths["prefix"]
    paths["bin"] = prefix / "bin"
    paths["lib"] = prefix / "lib"
    paths["mg5amc"] = prefix / "mg5amc"
    paths["pineappl"] = prefix / "pineappl"
    paths["cargo"] = prefix / "cargo"
    paths["lhapdf"] = prefix / "lhapdf"
    paths["lhapdf_data_alternative"] = prefix / "share" / "LHAPDF"

    return paths


def add_commands(paths):
    commands = {}

    commands["mg5"] = paths["mg5amc"] / "bin" / "mg5_aMC"
    commands["vrap"] = paths["prefix"] / "bin" / "Vrap"
    pineappl = shutil.which("pineappl")
    commands["pineappl"] = pathlib.Path(pineappl) if pineappl is not None else None

    return commands
