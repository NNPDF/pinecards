# -*- coding: utf-8 -*-
import copy
import os
import pathlib
import shutil
import tempfile
import warnings
from typing import Optional

import appdirs
import tomli

name = "runcardsrunner.toml"
"""Name of the config while (wherever it is placed)"""


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
    paths.append(pathlib.Path(appdirs.site_config_dir()))

    for p in paths:
        configs_file = p / name if p.is_dir() else p

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


# better to declare immediately the correct type
configs = {}
"Holds loaded configurations"


def add_scope(parent, scope_id, scope):
    "Do not override."
    newparent = copy.deepcopy(parent)
    # if the id not present, append the scope all at once
    if scope_id not in newparent:
        newparent[scope_id] = scope
    # if the id already present, preserve existing values
    else:
        for key, value in scope.items():
            # if already specified, do not override
            newparent[scope_id].setdefault(key, value)

    return newparent


def defaults(base_configs):
    """Provide additional defaults.

    Note
    ----
    The general rule is to never replace user provided input.

    """
    configs = add_paths(base_configs)
    configs = add_commands(configs)

    return configs


def add_paths(configs):
    root = configs["paths"]["root"]

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

    return add_scope(configs, "paths", paths)


def add_commands(configs):
    commands = {}

    commands["mg5"] = configs["paths"]["mg5amc"] / "bin" / "mg5_aMC"
    commands["vrap"] = configs["paths"]["prefix"] / "bin" / "Vrap"
    pineappl = shutil.which("pineappl")
    commands["pineappl"] = pathlib.Path(pineappl) if pineappl is not None else None

    return add_scope(configs, "commands", commands)
