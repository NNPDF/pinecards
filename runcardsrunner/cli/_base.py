# -*- coding: utf-8 -*-
import pathlib

import appdirs
import click
import tomli

from .. import configs

CONTEXT_SETTINGS = dict(help_option_names=["-h", "--help"])


@click.group(context_settings=CONTEXT_SETTINGS)
@click.option(
    "-c",
    "--configs",
    "cfg",
    default=None,
    type=click.Path(resolve_path=True, path_type=pathlib.Path),
    help="Explicitly specify config file (it has to be a valid TOML file).",
)
def command(cfg):
    base_configs = load(cfg)
    configs.configs = configs.defaults(base_configs)
    if cfg is not None:
        print(f"Configurations loaded from '{cfg}'")

    import rich

    rich.print(configs.configs)


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
        configs_file = p / configs.name if p.is_dir() else p

        if configs_file.is_file():
            return configs_file

    raise FileNotFoundError("No configurations file detected.")


def load(path=None):
    try:
        path = detect(path)
    except FileNotFoundError:
        if path is None:
            return {"paths": {"root": pathlib.Path.cwd()}}
        else:
            print("Configuration path specified is not valid.")
            quit()

    with open(path, "rb") as fd:
        loaded = tomli.load(fd)

    if "root" not in loaded["paths"]:
        loaded["paths"]["root"] = pathlib.Path(path).parent

    return loaded
