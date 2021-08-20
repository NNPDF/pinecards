import sys
import os
import subprocess
import shutil

import pkgconfig
import pygit2
import requests
import PyInquirer

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
mg5_exe = paths.mg5amc / "bin" / "mg5_aMC"

pineappl_repo = "https://github.com/N3PDF/pineappl.git"


def confirm(name):
    print(f"{name} not found")
    questions = [
        {
            "type": "confirm",
            "name": "install",
            "message": "Do you want to install it?",
        }
    ]
    answers = PyInquirer.prompt(questions)

    return answers["install"]


def mg5amc():
    # define availability condition
    condition = lambda: mg5_exe.exists() and os.access(mg5_exe, os.X_OK)

    if condition():
        print("✓ Found mg5amc")
        return True
    if not confirm("mg5amc"):
        return False

    print("Installing...")

    # download madgraph in prefix (if not present)
    subprocess.run(f"brz branch {mg5_repo} {paths.mg5amc}".split())

    # in case we're using python3, we need to convert the model file
    subprocess.run(f"{mg5_exe}", input=mg5_convert, encoding="ascii")

    # retest availability
    return condition()


def cargo():
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


def update_environ():
    def prepend(name, value):
        os.environ[name] = str(value) + os.pathsep + os.environ[name]

    pyver = ".".join(sys.version.split(".")[:2])
    prepend("PYTHONPATH", paths.prefix / "lib" / f"python{pyver}" / "site-packages")
    prepend("PATH", paths.prefix / "bin")
    prepend("LD_LIBRARY_PATH", paths.prefix / "lib")
    prepend("PKG_CONFIG_PATH", paths.prefix / "lib" / "pkgconfig")
