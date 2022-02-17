# -*- coding: utf-8 -*-
import copy
import pathlib
import shutil
import tempfile

configs = {}
"Holds loaded configurations"
name = "runcardsrunner.toml"
"Name of the config while (wherever it is placed)"


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
        runcards="runcards", theories="theories", prefix=".prefix"
    ).items():
        if key not in configs["paths"]:
            configs["paths"][key] = configs["paths"]["root"] / default
        elif pathlib.Path(configs["paths"][key]).anchor == "":
            configs["paths"][key] = configs["paths"]["root"] / configs["paths"][key]
        else:
            configs["paths"][key] = pathlib.Path(configs["paths"][key])

    configs["paths"]["rust_init"] = tempfile.mktemp()
    configs = add_prefix_paths(configs)

    return configs


def add_prefix_paths(configs):
    prefix = configs["paths"]["prefix"]
    paths = {}

    paths["bin"] = prefix / "bin"
    paths["lib"] = prefix / "lib"
    paths["mg5amc"] = prefix / "mg5amc"
    paths["pineappl"] = prefix / "pineappl"
    paths["cargo"] = prefix / "cargo"
    paths["lhapdf"] = prefix / "lhapdf"
    paths["lhapdf_data_alternative"] = prefix / "share" / "LHAPDF"

    prefix_scope = "_prefix"

    if prefix_scope not in configs["paths"]:
        configs["paths"][prefix_scope] = paths
    else:
        for key, path in paths.items():
            if key not in configs["paths"]["_prefix"]:
                configs["paths"]["_prefix"] = paths

    return configs


def add_commands(configs):
    commands = configs["commands"]

    commands["mg5"] = configs["paths"]["prefix"] / "bin" / "mg5_aMC"
    commands["pineappl"] = shutil.which("pineappl")
