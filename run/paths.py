import pathlib
import shutil

import lhapdf

pkg = pathlib.Path(__file__).absolute().parent
root = pkg.parent

# internal
cuts_code = pkg / "cuts_code"
cuts_variables = pkg / "cuts_variables"
lhapdf_conf = pkg / "confs" / "lhapdf.conf"

# external
runcards = root / "nnpdf31_proc"
patches = root / "patches"

# prefix  and locally installed
prefix = root / ".prefix"
bin = prefix / "bin"
lib = prefix / "lib"
mg5amc = prefix / "mg5amc"
mg5_exe = mg5amc / "bin" / "mg5_aMC"
pineappl = prefix / "pineappl"
cargo = prefix / "cargo"
pineappl_exe = lambda: pathlib.Path(shutil.which("pineappl"))

# tmp
rust_init = pathlib.Path("/tmp/rustup-init")

# lhapdf
lhapdf_data = prefix / "share" / "LHAPDF"
lhapdf.pathsPrepend(str(lhapdf_data))