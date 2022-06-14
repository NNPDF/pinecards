# -*- coding: utf-8 -*-
import pathlib
import shutil
import tempfile

pkg = pathlib.Path(__file__).absolute().parent
root = pkg.parent

# internal
lhapdf_conf = pkg / "confs" / "lhapdf.conf"

# external
runcards = root / "nnpdf31_proc"
theories = root / "theories"

# prefix  and locally installed
prefix = root / ".prefix"
bin = prefix / "bin"
lib = prefix / "lib"
mg5amc = prefix / "mg5amc"
mg5_exe = mg5amc / "bin" / "mg5_aMC"
vrap_exe = bin / "Vrap"
pineappl = prefix / "pineappl"
cargo = prefix / "cargo"
lhapdf_dir = prefix / "lhapdf"
# lhapdf data
lhapdf_data_alternative = prefix / "share" / "LHAPDF"


def pineappl_exe():
    p = shutil.which("pineappl")
    if p is not None:
        return pathlib.Path()
    else:
        raise OSError("pineappl installation not found.")


# tmp
rust_init = tempfile.mktemp()
