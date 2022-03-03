# -*- coding: utf-8 -*-
import copy
import pathlib
import shutil
import tempfile
import warnings

import appdirs
import tomli

name = "runcardsrunner.toml"
"Name of the config while (wherever it is placed)"


def detect(path=None):
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


def load(path=None):
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
            if key not in newparent[scope_id]:
                # else, add the default
                newparent[scope_id] = value

    return newparent


def defaults(base_configs):
    """Provide additional defaults.

    Note
    ----
    The general rule is to never replace user provided input.
    """
    configs = copy.deepcopy(base_configs)

    configs = add_paths(configs)
    configs = add_commands(configs)

    return configs


def add_paths(configs):
    for key, default in dict(
        runcards="runcards",
        theories="theories",
        prefix=".prefix",
        results="results",
    ).items():
        if key not in configs["paths"]:
            configs["paths"][key] = configs["paths"]["root"] / default
        elif pathlib.Path(configs["paths"][key]).anchor == "":
            configs["paths"][key] = configs["paths"]["root"] / configs["paths"][key]
        else:
            configs["paths"][key] = pathlib.Path(configs["paths"][key])

    configs["paths"]["rust_init"] = pathlib.Path(tempfile.mktemp())
    configs = add_prefix_paths(configs)

    return configs


def add_prefix_paths(configs):
    configs = copy.deepcopy(configs)
    prefix = configs["paths"]["prefix"]
    paths = {}

    paths["bin"] = prefix / "bin"
    paths["lib"] = prefix / "lib"
    paths["mg5amc"] = prefix / "mg5amc"
    paths["pineappl"] = prefix / "pineappl"
    paths["cargo"] = prefix / "cargo"
    paths["lhapdf"] = prefix / "lhapdf"
    paths["lhapdf_data_alternative"] = prefix / "share" / "LHAPDF"

    prefix_scope = "prefixed"

    configs["paths"] = add_scope(configs["paths"], prefix_scope, paths)
    return configs


def add_commands(configs):
    commands = {}

    commands["mg5"] = configs["paths"]["prefixed"]["mg5amc"] / "bin" / "mg5_aMC"
    commands["vrap"] = configs["paths"]["prefix"] / "bin" / "Vrap"
    pineappl = shutil.which("pineappl")
    commands["pineappl"] = pathlib.Path(pineappl) if pineappl is not None else None

    return add_scope(configs, "commands", commands)
