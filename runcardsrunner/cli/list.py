import click
import rich
import rich.markdown

from .. import paths, tools
from ._base import command


@command.group("list")
def subcommand():
    """List available resources."""


@subcommand.command()
def runcards():
    """List available runcards."""
    main(paths.runcards)


@subcommand.command()
def theories():
    """List available theories."""
    main(paths.theories, files=True, prefix="theory_")


@subcommand.group()
def mg5():
    """MG5aMC@NLO related assets."""


@mg5.command()
def patches():
    """List available patches."""
    main(paths.patches, files=True)


@mg5.command()
def cuts():
    """List available cuts."""
    main(paths.cuts_code, files=True)


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