# -*- coding: utf-8 -*-
import os
import pathlib
import shutil
import subprocess
import sys
import tarfile
import tempfile

import lhapdf_management
import pkgconfig
import pygit2
import requests

from . import configs, tools
from .external import vrap

MG5_REPO = "lp:~maddevelopers/mg5amcnlo/3.3.1"
"bazaar/breeze repo location for MG5aMC\\@NLO"
MG5_CONVERT = """
set auto_convert_model True
import model loop_qcd_qed_sm_Gmu
quit
"""
"Instructions to set the correct model for MG5aMC\\@NLO."

PINEAPPL_REPO = "https://github.com/N3PDF/pineappl.git"
"Git repo location for pineappl."

LHAPDF_VERSION = "LHAPDF-6.4.0"
"Version of LHAPDF to be used by default (if not already available)."


def init_prefix():
    configs.configs["paths"]["prefix"].mkdir(exist_ok=True)
    configs.configs["paths"]["bin"].mkdir(exist_ok=True)
    configs.configs["paths"]["lib"].mkdir(exist_ok=True)


def is_exe(command: os.PathLike) -> bool:
    """Check if given path exists and is executable."""
    command = pathlib.Path(command)
    return command.exists() and os.access(command, os.X_OK)


def mg5amc():
    """Initialize `MadGraph5_aMC\\@NLO <https://code.launchpad.net/mg5amcnlo>`_.

    Returns
    -------
    bool
        whether the main executable is now existing.

    """
    mg5 = configs.configs["commands"]["mg5"]

    if is_exe(mg5):
        print("✓ Found mg5amc")
        return True

    print("Installing...")

    # download madgraph in prefix (if not present)
    subprocess.run(
        f"brz branch {MG5_REPO} {configs.configs['paths']['mg5amc']}".split()
    )

    # in case we're using python3, we need to convert the model file
    subprocess.run(f"{mg5}", input=MG5_CONVERT, encoding="ascii")

    # retest availability
    return is_exe(mg5)


def hawaiian_vrap():
    """Install a version of vrap flavoured with pineappl
    from https://github.com/NNPDF/hawaiian_vrap

    Returns
    -------
    bool
        whether vrap is now installed
    """
    vrapx = configs.configs["commands"]["vrap"]

    if is_exe(vrapx):
        print("✓ Found vrap")
        return True

    url = f"https://github.com/NNPDF/hawaiian_vrap/archive/refs/tags/{vrap.VERSION}.tar.gz"
    print(f"Installing the version {vrap.VERSION} of vrap from {url}")

    with tempfile.TemporaryDirectory() as tmp:
        tmp_path = pathlib.Path(tmp)
        vrap_tar = tmp_path / f"hawaiian_vrap-{vrap.VERSION}.tar.gz"
        with requests.get(url) as r:
            vrap_tar.write_bytes(r.content)

        with tarfile.open(vrap_tar, "r:gz") as tar:
            tar.extractall(tmp_path)

        # Compile vrap
        tmp_vrap = tmp_path / f"hawaiian_vrap-{vrap.VERSION}"
        subprocess.run("autoreconf -fiv", cwd=tmp_vrap / "src", shell=True, check=True)
        build_dir = tmp_vrap / "build"
        build_dir.mkdir(exist_ok=True)
        subprocess.run(
            ["../src/configure", "--prefix", configs.configs["paths"]["prefix"]],
            cwd=build_dir,
            check=True,
        )
        subprocess.run(["make", "install"], cwd=build_dir, check=True)

    return is_exe(vrapx)


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

    cargo_home = configs.configs["paths"]["cargo"]

    # if there is not a user cargo update environment
    os.environ["CARGO_HOME"] = str(cargo_home)
    if cargo_home.is_dir():
        return str(cargo_home / "bin" / "cargo")

    rust_init = configs.configs["paths"]["rust_init"]

    # if cargo not available let's install
    with requests.get("https://sh.rustup.rs") as r:
        with open(rust_init, "wb") as f:
            f.write(r.content)
    # install location is controlled by CARGO_HOME variable
    subprocess.run(f"bash {rust_init} --profile minimal --no-modify-path -y".split())

    return str(cargo_home / "bin" / "cargo")


def pineappl(capi=True, cli=False):
    """Initialize `PineAPPL <https://github.com/N3PDF/pineappl>`_.

    Parameters
    ----------
    capi : bool
        whether to install PineAPPl CAPI (by default `True`, since it's the
        only thing required)
    cli : bool
        whether to install even PineAPPL CLI (by default `False`, since it's
        not required to run)

    Returns
    -------
    bool
        whether `pineappl` and  `pineappl_capi` are now available.

    """
    # define availability condition
    def installed():
        return pkgconfig.exists("pineappl_capi")

    def cli_installed():
        return shutil.which("pineappl") is not None

    # check if there is something to do at all
    if (not capi or installed()) and (not cli or cli_installed()):
        print("✓ Found pineappl")
        return True

    print("Installing...")

    if not pkgconfig.exists("lhapdf"):
        lhapdf()

    if capi and not installed():
        try:
            repo = pygit2.Repository(configs.configs["paths"]["pineappl"])
            tools.git_pull(repo)
        except pygit2.GitError:
            repo = pygit2.clone_repository(
                PINEAPPL_REPO, configs.configs["paths"]["pineappl"]
            )

        cargo_exe = cargo()
        subprocess.run([cargo_exe] + "install --force cargo-c".split())

        subprocess.run(
            [cargo_exe]
            + "cinstall --release --prefix".split()
            + [
                str(configs.configs["paths"]["prefix"]),
                "--manifest-path=pineappl_capi/Cargo.toml",
            ],
            cwd=configs.configs["paths"]["pineappl"],
        )

    if cli and not cli_installed():
        cargo_exe = cargo()
        subprocess.run(
            [cargo_exe]
            + "install --path pineappl_cli --root".split()
            + [str(configs.configs["paths"]["prefix"])],
            cwd=configs.configs["paths"]["pineappl"],
        )
        configs.configs["commands"]["pineappl"] = shutil.which("pineappl")

    # retest availability
    return installed() and (not cli or cli_installed())


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

    lhapdf_data = configs.configs["paths"]["lhapdf_data_alternative"]
    lhapdf_data.mkdir(parents=True, exist_ok=True)
    shutil.copy2(
        pathlib.Path(__file__).absolute().parent / "confs" / "lhapdf.conf", lhapdf_data
    )
    update_lhapdf_path(lhapdf_data)


def lhapdf():
    """Install `LHAPDF <https://lhapdf.hepforge.org/>`_ C++ library.

    Not needed:
        - for `mg5`, since it's vendored
        - for `yadism`, since we depend on the PyPI version
    """
    # define availability condition
    def installed():
        try:
            # test python package availability
            import lhapdf  # pylint: disable=unused-import
        except ModuleNotFoundError:
            return False
        return pkgconfig.exists("lhapdf")

    # check if there is something to do at all
    if installed():
        print("✓ Found lhapdf")
        return True

    lhapdf_dest = configs.configs["paths"]["lhapdf"]
    lhapdf_tar = lhapdf_dest / (LHAPDF_VERSION + ".tar.gz")
    lhapdf_code = lhapdf_dest / LHAPDF_VERSION

    lhapdf_dest.mkdir(exist_ok=True)
    with requests.get(
        f"https://lhapdf.hepforge.org/downloads/?f={lhapdf_tar.name}"
    ) as r:
        with open(lhapdf_tar, "wb") as f:
            f.write(r.content)

    with tarfile.open(lhapdf_tar, "r:gz") as tar:

        tar.extractall(lhapdf_dest)

    env = os.environ.copy()
    env["PYTHON"] = sys.executable
    subprocess.run(
        f"./configure --prefix={configs.configs['paths']['prefix']}".split(),
        env=env,
        cwd=lhapdf_code,
    )
    subprocess.run("make", cwd=lhapdf_code)
    subprocess.run("make install".split(), cwd=lhapdf_code)

    return installed()


def update_environ():
    """Adjust necessary environment files."""

    def prepend(name, value):
        if name not in os.environ:
            os.environ[name] = ""
        os.environ[name] = str(value) + os.pathsep + os.environ[name]

    lib = configs.configs["paths"]["lib"]
    pyver = ".".join(sys.version.split(".")[:2])
    prepend(
        "PYTHONPATH",
        lib / f"python{pyver}" / "site-packages",
    )
    prepend("PATH", configs.configs["paths"]["bin"])
    prepend("LD_LIBRARY_PATH", lib)
    prepend("PKG_CONFIG_PATH", lib / "pkgconfig")
