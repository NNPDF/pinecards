# -*- coding: utf-8 -*-
import pathlib
import shutil
import tempfile

import rich

name = "runcardsrunner.toml"
"Name of the config while (wherever it is placed)"


class Configurations:
    def __init__(self, dictionary=None):
        if isinstance(dictionary, Configurations):
            self._dict = dictionary._dict
        elif dictionary is None:
            self._dict = {}
        else:
            self._dict = dictionary

    def __repr__(self):
        return self._dict.__repr__()

    def __getattribute__(self, name):
        if name[0] == "_":
            return super().__getattribute__(name)

        value = self._dict[name]
        if isinstance(value, dict):
            value = Configurations(value)
        return value

    def __getitem__(self, key):
        return self.__getattribute__(key)

    def __setattribute__(self, name, value):
        self._dict[name] = value

    def __setitem__(self, key, value):
        if key[0] == "_":
            raise LookupError(
                "Elements with leading '_' can not be retrieved later, so you"
                f" can not set (attempted: '{key}')"
            )

        self._dict[key] = value

    def __contains__(self, item):
        return item in self._dict

    def _pprint(self):
        rich.print(self._dict)


def add_scope(base, scope_id, scope):
    "Do not override."
    if scope_id not in base:
        base[scope_id] = scope
    else:
        for key, value in scope.items():
            if key not in base[scope_id]:
                base[scope_id] = value


configs = Configurations()
"Holds loaded configurations"


def defaults(base_configs):
    """Provide additional defaults.

    Note
    ----
    The general rule is to never replace user provided input.
    """
    configs = Configurations(base_configs)

    configs = add_paths(configs)
    configs = add_commands(configs)

    return Configurations(configs)


def add_paths(configs):
    for key, default in dict(
        runcards="runcards", theories="theories", prefix=".prefix"
    ).items():
        if key not in configs.paths:
            configs.paths[key] = configs.paths.root / default
        elif pathlib.Path(configs.paths[key]).anchor == "":
            configs.paths[key] = configs.paths.root / configs.paths[key]
        else:
            configs.paths[key] = pathlib.Path(configs.paths[key])

    configs.paths["rust_init"] = tempfile.mktemp()
    configs = add_prefix_paths(configs)

    return configs


def add_prefix_paths(configs):
    prefix = configs.paths.prefix
    paths = {}

    paths["bin"] = prefix / "bin"
    paths["lib"] = prefix / "lib"
    paths["mg5amc"] = prefix / "mg5amc"
    paths["pineappl"] = prefix / "pineappl"
    paths["cargo"] = prefix / "cargo"
    paths["lhapdf"] = prefix / "lhapdf"
    paths["lhapdf_data_alternative"] = prefix / "share" / "LHAPDF"

    prefix_scope = "prefixed"
    add_scope(configs.paths, prefix_scope, paths)

    return configs


def add_commands(configs):
    commands = {}

    commands["mg5"] = configs.paths.prefix / "bin" / "mg5_aMC"
    pineappl = shutil.which("pineappl")
    commands["pineappl"] = pathlib.Path(pineappl) if pineappl is not None else None

    add_scope(configs, "commands", commands)

    return configs
