import os
import subprocess
import io

from . import paths


paths.prefix.mkdir(exist_ok=True)


def install_mg5amc():
    mg5_repo = "lp:~maddevelopers/mg5amcnlo/3.2.0"
    mg5_convert = """
set auto_convert_model True
import model loop_qcd_qed_sm_Gmu
quit
"""
    mg5_exe = paths.mg5amc / "bin" / "mg5_aMC"

    # download madgraph in prefix (if not present)
    if not (mg5_exe.exists() and os.access(mg5_exe, os.X_OK)):
        subprocess.run(f"brz branch {mg5_repo} {paths.mg5amc}".split())

    # in case we're using python3, we need to convert the model file
    subprocess.run(f"{mg5_exe}", input=mg5_convert, encoding="ascii")
