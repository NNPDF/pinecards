import pathlib

pkg = pathlib.Path(__file__).absolute().parent
root = pkg.parent
runcards = root / "nnpdf31_proc"
prefix = root / ".prefix"
mg5amc = prefix / "mg5amc"
