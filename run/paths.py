import pathlib

pkg = pathlib.Path(__file__).absolute().parent
root = pkg.parent

runcards = root / "nnpdf31_proc"

# prefix  and locally installed
prefix = root / ".prefix"
bin = prefix / "bin"
lib = prefix / "lib"
mg5amc = prefix / "mg5amc"
pineappl = prefix / "pineappl"
cargo = prefix / "cargo"

# tmp
rust_init = pathlib.Path("/tmp/rustup-init")
