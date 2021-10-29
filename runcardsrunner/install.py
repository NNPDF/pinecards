import os
import pathlib
import shutil
import subprocess
import sys
import tarfile

import lhapdf_management
import pkgconfig
import pygit2
import requests

from . import paths, tools

paths.prefix.mkdir(exist_ok=True)
paths.bin.mkdir(exist_ok=True)
paths.lib.mkdir(exist_ok=True)

mg5_repo = "lp:~maddevelopers/mg5amcnlo/3.1.2"
"bazaar/breeze repo location for MG5aMC\\@NLO"
mg5_convert = """
set auto_convert_model True
import model loop_qcd_qed_sm_Gmu
quit
"""
"instructions to set the correct model for MG5aMC\\@NLO"

pineappl_repo = "https://github.com/N3PDF/pineappl.git"
"git repo location for pineappl"


def mg5amc():
    """Initialize `MadGraph5_aMC\\@NLO <https://code.launchpad.net/mg5amcnlo>`_.

    Returns
    -------
    bool
        whether the main executable is now existing.

    """
    # define availability condition
    condition = lambda: paths.mg5_exe.exists() and os.access(paths.mg5_exe, os.X_OK)

    if condition():
        print("✓ Found mg5amc")
        return True

    print("Installing...")

    # download madgraph in prefix (if not present)
    subprocess.run(f"brz branch {mg5_repo} {paths.mg5amc}".split())

    # in case we're using python3, we need to convert the model file
    subprocess.run(f"{paths.mg5_exe}", input=mg5_convert, encoding="ascii")

    # retest availability
    return condition()


def cargo():
    """Initialize `Rust <https://www.rust-lang.org/>`_ and `Cargo
    <https://doc.rust-lang.org/stable/cargo/>`_.

    Returns
    -------
    str
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


def pineappl(cli=False):
    """Initialize `PineAPPL <https://github.com/N3PDF/pineappl>`_.

    Parameters
    ----------
    cli : bool
        whether to install even PineAPPL CLI (by default `False`, since it's
        not required to run)

    Returns
    -------
    bool
        whether `pineappl` and  `pineappl_capi` are now available.

    """
    # define availability condition
    installed = lambda: pkgconfig.exists("pineappl_capi")
    cli_installed = lambda: shutil.which("pineappl") is not None

    if installed() and (not cli or cli_installed()):
        print("✓ Found pineappl")
        return True

    print("Installing...")

    if not pkgconfig.exists("lhapdf"):
        lhapdf()

    if not installed():
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

    if cli and not cli_installed():
        cargo_exe = cargo()
        subprocess.run(
            [cargo_exe]
            + "install --path pineappl_cli --root".split()
            + [str(paths.prefix)],
            cwd=paths.pineappl,
        )

    # retest availability
    return condition()


def update_lhapdf_path(path):
    """Update LHAPDF path, both in environment and `lhapdf_management
    <https://pypi.org/project/lhapdf-management/>`_

    Parameters
    ----------
    path : str or pathlib.Path
        path to LHAPDF data

    """
    os.environ["LHAPDF_DATA_PATH"] = str(path)
    lhapdf_management.environment.datapath = pathlib.Path(path)


def lhapdf_conf(pdf):
    """Initialize `LHAPDF <https://lhapdf.hepforge.org/>`_.

    Parameters
    ----------
    pdf : str
        LHAPDF name of the required PDF

    """
    if shutil.which("lhapdf-config") is not None or pkgconfig.exists("lhapdf"):
        lhapdf_data = pathlib.Path(
            subprocess.run("lhapdf-config --datadir".split(), capture_output=True)
            .stdout.decode()
            .strip()
        )
        if not lhapdf_data.exists():
            lhapdf_data = (
                pathlib.Path(pkgconfig.variables("lhapdf")["datarootdir"]).absolute()
                / "LHAPDF"
            )
        update_lhapdf_path(lhapdf_data)
        # attempt to determine if it is possible to get the required PDF in the
        # existing folder (if possible return)
        try:
            if os.access(lhapdf_data, os.W_OK) or pdf in (
                x.name for x in lhapdf_management.pdf_list("--installed")
            ):
                return
        except PermissionError:
            pass
    paths.lhapdf_data_alternative.mkdir(parents=True, exist_ok=True)
    shutil.copy2(paths.lhapdf_conf, paths.lhapdf_data_alternative)
    update_lhapdf_path(paths.lhapdf_data_alternative)


def lhapdf():
    """Install `LHAPDF <https://lhapdf.hepforge.org/>`_ C++ library."""
    version = "LHAPDF-6.4.0"
    lhapdf_tar = paths.lhapdf_dir / (version + ".tar.gz")
    lhapdf_code = paths.lhapdf_dir / version

    paths.lhapdf_dir.mkdir(exist_ok=True)
    with requests.get(
        f"https://lhapdf.hepforge.org/downloads/?f={lhapdf_tar.name}"
    ) as r:
        with open(lhapdf_tar, "wb") as f:
            f.write(r.content)

    with tarfile.open(lhapdf_tar, "r:gz") as tar:
        tar.extractall(paths.lhapdf_dir)

    env = os.environ.copy()
    env["PYTHON"] = sys.executable
    subprocess.run(
        f"./configure --prefix={paths.prefix}".split(), env=env, cwd=lhapdf_code
    )
    subprocess.run("make", cwd=lhapdf_code)
    subprocess.run("make install".split(), cwd=lhapdf_code)


def update_environ():
    """Adjust necessary environment files."""

    def prepend(name, value):
        if name not in os.environ:
            os.environ[name] = ""
        os.environ[name] = str(value) + os.pathsep + os.environ[name]

    pyver = ".".join(sys.version.split(".")[:2])
    prepend("PYTHONPATH", paths.prefix / "lib" / f"python{pyver}" / "site-packages")
    prepend("PATH", paths.prefix / "bin")
    prepend("LD_LIBRARY_PATH", paths.prefix / "lib")
    prepend("PKG_CONFIG_PATH", paths.prefix / "lib" / "pkgconfig")
