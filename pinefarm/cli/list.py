# -*- coding: utf-8 -*-
import rich
import rich.markdown

from .. import configs, external
from ._base import command


@command.group("list")
def subcommand():
    """List available resources."""


@subcommand.command()
def runcards():
    """List available runcards."""
    main(configs.configs["paths"]["runcards"])


@subcommand.command()
def theories():
    """List available theories."""
    main(configs.configs["paths"]["theories"], files=True, prefix="theory_")


@subcommand.group()
def mg5():
    """MG5aMC@NLO related assets."""


@mg5.command()
def patches():
    """List available patches."""
    main(external.mg5.paths.patches, files=True)


@mg5.command()
def cuts():
    """List available cuts."""
    main(external.mg5.paths.cuts_code, files=True)


def main(path, files=False, prefix=""):
    report = ""
    for p in sorted(path.glob("*")):
        name = None
        if p.is_dir():
            name = p.name
        if files and p.is_file():
            name = p.stem

        if name is not None:
            report += f"- {name}\n".replace(prefix, "")

    rich.print(rich.markdown.Markdown(report))
