import os
import shutil
import subprocess
import sys

import InquirerPy
import pkgconfig
import pygit2
import requests

from . import paths, tools

paths.prefix.mkdir(exist_ok=True)
paths.bin.mkdir(exist_ok=True)
paths.lib.mkdir(exist_ok=True)

mg5_repo = "lp:~maddevelopers/mg5amcnlo/3.2.0"
mg5_convert = """
set auto_convert_model True
import model loop_qcd_qed_sm_Gmu
quit
"""

pineappl_repo = "https://github.com/N3PDF/pineappl.git"


def confirm(name):
    """
    Ask for the confirmation to install a given program.

    Parameters
    ----------
        name : str
            program name

    Returns
    -------
        bool :
            install the program?
    """
    print(f"{name} not found")
    questions = [
        {
            "type": "confirm",
            "name": "install",
            "message": "Do you want to install it?",
        }
    ]
    answers = InquirerPy.prompt(questions)

    return answers["install"]


def mg5amc():
    """
    Initialize `MadGraph5_aMC@NLO <https://code.launchpad.net/mg5amcnlo>`_.

    Returns
    -------
        callable :
            condition to check whether the main executable exists.
    """
    # define availability condition
    condition = lambda: paths.mg5_exe.exists() and os.access(paths.mg5_exe, os.X_OK)

    if condition():
        print("✓ Found mg5amc")
        return True
    if not confirm("mg5amc"):
        return False

    print("Installing...")

    # download madgraph in prefix (if not present)
    subprocess.run(f"brz branch {mg5_repo} {paths.mg5amc}".split())

    # in case we're using python3, we need to convert the model file
    subprocess.run(f"{paths.mg5_exe}", input=mg5_convert, encoding="ascii")

    # retest availability
    return condition()


def cargo():
    """
    Initialize `Rust <https://www.rust-lang.org/>`_.

    Returns
    -------
        str :
            path to `cargo`
    """
    # look for existing cargo
    cargo_exe = shutil.which("cargo")

    # found, exit
    if cargo_exe is not None:
        return cargo_exe

    # if there is not a user cargo update environment
    os.environ["CARGO_HOME"] = str(paths.cargo)
    if paths.cargo.is_dir():
        return str(paths.cargo / "bin" / "cargo")

    # if cargo not available let's install
    with requests.get("https://sh.rustup.rs") as r:
        with open(paths.rust_init, "wb") as f:
            f.write(r.content)
    # install location is controlled by CARGO_HOME variable
    subprocess.run(
        f"bash {paths.rust_init} --profile minimal --no-modify-path -y".split()
    )

    return str(paths.cargo / "bin" / "cargo")


def pineappl():
    """
    Initialize `PineAPPL <https://github.com/N3PDF/pineappl>`_.

    Returns
    -------
        callable :
            condition to check whether `pineappl` and  `pineappl_capi` are available.
    """
    # define availability condition
    condition = lambda: shutil.which("pineappl") is not None and pkgconfig.exists(
        "pineappl_capi"
    )

    if condition():
        print("✓ Found pineappl")
        return True
    if not confirm("pineappl"):
        return False

    print("Installing...")

    try:
        repo = pygit2.Repository(paths.pineappl)
        tools.git_pull(repo)
    except pygit2.GitError:
        repo = pygit2.clone_repository(pineappl_repo, paths.pineappl)

    cargo_exe = cargo()
    subprocess.run([cargo_exe] + "install --force cargo-c".split())

    subprocess.run(
        [cargo_exe]
        + "cinstall --release --prefix".split()
        + [str(paths.prefix), "--manifest-path=pineappl_capi/Cargo.toml"],
        cwd=paths.pineappl,
    )
    subprocess.run(
        [cargo_exe]
        + "install --path pineappl_cli --root".split()
        + [str(paths.prefix)],
        cwd=paths.pineappl,
    )

    # retest availability
    return condition()


def lhapdf():
    """Initialize `LHAPDF <https://lhapdf.hepforge.org/>`_."""
    paths.lhapdf_data.mkdir(parents=True, exist_ok=True)
    shutil.copy2(paths.lhapdf_conf, paths.lhapdf_data)


def update_environ():
    """Adjust necessary environment files."""

    def prepend(name, value):
        os.environ[name] = str(value) + os.pathsep + os.environ[name]

    pyver = ".".join(sys.version.split(".")[:2])
    prepend("PYTHONPATH", paths.prefix / "lib" / f"python{pyver}" / "site-packages")
    prepend("PATH", paths.prefix / "bin")
    prepend("LD_LIBRARY_PATH", paths.prefix / "lib")
    prepend("PKG_CONFIG_PATH", paths.prefix / "lib" / "pkgconfig")
