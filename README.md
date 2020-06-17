# aMCblast
The madgraph/aMCblast project consists of a set of tools for the automated
generation of APPLgrids that include NLO(QCD+EW) corrections (mainly for NNPDF
fit purposes).

## Prerequisites
To successfully generate an APPLgrid, the following packages are required:

* ROOT, which can downloaded from <https://root.cern.ch>. Follow the
  instructions there, in particular pay attention to sourcing `thisroot.sh`, if
  this is needed in your case.
* `ninja` and `meson`, which are required to build `applgridphoton`. Both
  packages can be installed using pip. Make sure you use a recent version of
  both.
* The modfied version of **APPLgrid** available in the `applgridphoton` folder,
  which must be compiled and installed. Use the following commands:

      cd applgridphoton
      meson build -Dprefix=${prefix} -Dlibdir=lib
      cd build
      ninja && ninja install

  Make sure that `${prefix}` points to the proper installation directory, and that
  `PATH` includes `${prefix}/bin`, where the binaries are installed. Finally,
  make sure to also export/update `PKG_CONFIG_PATH` to
  `${prefix}/lib/pkgconfig`, so that the library and header can be found by
  `mg5_aMC`.
* the modified version of **Madgraph/MC@NLO**
  (*https://code.launchpad.net/~amcblast/+junk/3.0.2*) installable as `bzr
  branch lp:~amcblast/+junk/3.0.2` (bazaar is available from
  http://bazaar.canonical.com/en/ if it is not already installed on your
  machine). The binary `mg5_aMC` must also be found in `PATH`.

## Datasets
After a successful installation of the prerequisites above, the following files
must be present for each data set. These files must be located in the
subdirectory `nnpdf31_proc/DATASET` folder and, where `DATASET` should be named
after the corresponding data set included in a NNPDF fit. The contents and
meaning are as follows:

* The `output.txt` file (compulsory). This file contains the instructions to
  generate the relevant process. For details, please see
  [arXiv:1804.10017](http://arxiv.org/abs/arXiv:1804.10017) and
  [arXiv:1405.0301](http://arxiv.org/abs/arXiv:1405.0301). The variable
  `@OUTPUT@` should be used to generate the directory containing the source
  files.

* The `launch.txt` file (compulsory). This file contains the instructions to
  run the relevant process, including the relevant physical parameters. Since
  the parameter values are inserted by `run.sh`, do not insert the numerical
  values into the text file but rather the run variables. Currently supported
  ones are `@MZ@`, `@MW@`, and `@YMT@`. The names are the same as chosen by
  `mg5_aMC`, but written in uppercase and surrounded with `@`. For details
  about more parameters, please see the `Template/NLO/Cards/run_card.dat` file
  in Madgraph/MC@NLO.

* The `analysis.f` file (compulsory). This Fortran file contains the
  instructions for the kinematic analysis in the `HwU` format and must fill the
  APPLgrid file. Examples can be found in the
  `Template/NLO/FixedOrderAnalysis/` folder of Madgraph/MC@NLO.

* The `*.patch` file(s) (optional). These are one or more `.patch` files that
  are applied after Madgraph/MC@NLO has generated the sources. For instance, to
  use a dynamical scale, a patch modifying `setscales.f` file should be
  included in the directory. To create patches use the command `diff -Naurb
  original new > patch.patch`. The patches are applied in an unspecified order,
  using `patch -p1 ...`.

## Generating the APPLgrid(s)
Provided the above files, the production of the grids only requires the user to
run the `./run.sh [dataset]` script. The script takes as only parameter the
dataset, which is the name of the subdirectory containing the all the files
described before. To list all possibilities of `[dataset]` simply run the
script without parameters.

## Where's my output?
After having run, `./run.sh [dataset]` (see above), the script prints some
output useful to quickly validate the contents of the grid. The last shows the
directory where all results are stored, which has the form `dataset-date`,
where `date` is a numerical date when the generation was started.

The contents of this directory are:

* applcheck.log: The output of running `applcheck` with the grid and the
  correct PDF set
* `DATSET`: The directory created by `mg5_aMC`. A few interesting files in
  subdirectories are:
  * `Events/*/MADatNLO.HwU`: histograms with uncertainties (HwU)
  * `Events/*/amcblast_obs_*.root`: grids created by `mg5_aMC`, not yet merged
    together
* `DATASET.{pineappl,root}`: All grids created by `mg5_aMC` merged together
* `launch.log`: Output of `mg5_aMC` during the 'launch' phase
* `launch.txt`: Run card for the launch phase, with all variables substituted
  to their final values
* `output.log`: Output of `mg5_aMC` during the 'output' phase
* `output.txt`: Run card for the output phase, with all variables substituted
  to their final values
* `results.log`: The numerical results of the run, comparing the results of the
  grid against the results from `mg5_aMC`
* `time.log`: Total `time` needed for the run

## Switching to PineAPPL and back to the APPLgrid-based code
To use the grid generation tools, a third repository is needed, which can be
found at <https://github.com/N3PDF/pineappl>. Download this repository, and run

    cargo build --release

If `cargo` is not found, you first need to install Rust. Use either your
package manager or follow the official instructions at
<https://www.rust-lang.org/tools/install>. The above command will build the
following file in the `PineAPPL` repository:

    target/release/libpineappl_capi.so

Copy this file over the old one installed by `applgridphoton` in
`${prefix}/lib`, where `${prefix}` is the installation directory. You can then
use `PineAPPL` to generate grids using `./run.sh` as usual.

To switch back to the APPLgrid-based code, simply reinstall `applgridphoton` or
copy it's `libpineappl_capi.so` from the `build` folder (of `applgridphoton`)
into the installation directory.

Note that generated grids are different from each other. Grids generated with
`applgridphoton` have the `.root` extension and grids generated with `PineAPPL`
have the `.pineappl` extension (only the merged grid).
