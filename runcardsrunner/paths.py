import pathlib
import shutil

import lhapdf

pkg = pathlib.Path(__file__).absolute().parent
root = pkg.parent

# internal
cuts_code = pkg / "external" / "mg5" / "cuts_code"
cuts_variables = pkg / "external" / "mg5" / "cuts_variables"
lhapdf_conf = pkg / "confs" / "lhapdf.conf"

# external
runcards = root / "nnpdf31_proc"
theories = root / "theories"
patches = root / "patches"

# prefix  and locally installed
prefix = root / ".prefix"
bin = prefix / "bin"
lib = prefix / "lib"
mg5amc = prefix / "mg5amc"
mg5_exe = mg5amc / "bin" / "mg5_aMC"
pineappl = prefix / "pineappl"
cargo = prefix / "cargo"
lhapdf_dir = prefix / "lhapdf"
pineappl_exe = lambda: pathlib.Path(shutil.which("pineappl"))

# tmp
rust_init = pathlib.Path("/tmp/rustup-init")

# lhapdf data
lhapdf_data_alternative = prefix / "share" / "LHAPDF"
