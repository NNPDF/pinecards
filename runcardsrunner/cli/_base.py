# -*- coding: utf-8 -*-
import pathlib
import warnings

import click

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
    try:
        cfgpath = configs.detect(cfg)
    except FileNotFoundError:
        warnings.warn("No configuration file detected.")
        cfgpath = None

    base_configs = configs.load(cfgpath)
    configs.configs = configs.defaults(base_configs)
    if cfg is not None:
        print(f"Configurations loaded from '{cfg}'")
