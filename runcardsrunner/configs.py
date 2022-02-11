import pathlib

import appdirs
import tomli

name = "runcardsrunner.toml"


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
        configs = p / name
        if configs.is_file():
            return configs

    raise FileNotFoundError("No configurations file detected.")


def load(path=None):
    if path is None:
        try:
            path = detect()
        except FileNotFoundError:
            return None

    with open(path, "rb") as fd:
        return tomli.load(fd)
