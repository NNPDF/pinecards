# aMCblast
The madgraph/aMCblast project consists of a set of tools for the automated
generation of PineAPPL grids that include NLO QCD+EW corrections.

## Prerequisites
To successfully generate a PineAPPL grid, the following packages are required:

* the modified version of **Madgraph/MC@NLO**
  (<https://launchpad.net/mg5amc-pineappl>) installable as `bzr branch
  lp:~amcblast/mg5amc-pineappl/trunk` (bazaar is available from
  <http://bazaar.canonical.com/en/> if it is not already installed on your
  machine). The binary `mg5_aMC` must also be found in `PATH`.
* the **Rust** tools, see <https://www.rust-lang.org/tools/install>.
* the **PineAPPL C API**: download <https://github.com/N3PDF/pineappl>, then
  execute the following steps inside the repository:

      cd pineappl_capi
      cargo cinstall --release --prefix=${prefix}
      cd ..

  Make sure to replace `${prefix}` with an appropriate installation directory,
  and that the environment variables are set (permanently in your `~/.bashr`,
  for examples) to the following values

      export LD_LIBRARY_PATH=${prefix}/lib
      export PKG_CONFIG_PATH=${prefix}/lib/pkgconfig

  Test your installation using

      pkg-config pineappl_capi --libs

  This prints the linker flags needed to link against the C API of PineAPPL,
  and should like similar to the following output

      -L${prefix}/lib -lpineappl_capi

  If there is no output, something is wrong.
* in the same repository, install the shell program **`pineappl`** using

      cargo install --path pineappl_cli

  Note that this will install the program for the current user. If you want to
  install the program system-wide use the option `--root` with the proper
  directory.

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

* The `postrun.sh` file (optional). This is a BASH script which is run after
  the successful generation of the PineAPPL grid and can be used to perform
  additional operations, such as rescaling. The environment variable `$GRID`
  contains the relative path the PineAPPL grid.

* The `metadata.txt` file (optional). This file collects all metadata, which is
  written into the grid after generation. Arbitrary `key=value` pairs are
  supported, the most common are:

  - `arxiv`: The arxiv number of the experimental analysis, or if there are
    more than one, comma-seperated numbers.
  - `description`: A short description of the process/observables. Make sure to
    include also the name of the experiment and the centre-of-mass energy.
  - `hepdata`: The DOI pointing to the experimental data.
  - `x1_label`: The name of first (second, third, ...) observable.
  - `x1_label_tex`: The name of the observable, but TeX commands are allowed.
  - `x1_unit`: The unit of the observable (typically `GeV`). If this key is not
    present, the corresponding observable is assumed to be dimensionless.
  - `y_label`: The unit for the cross section (typically `pb`).

## Generating the PineAPPL grid(s)
Provided the above files, the production of the grids only requires the user to
run the `./run.sh [dataset]` script. The script takes as only parameter the
dataset, which is the name of the subdirectory containing the all the files
described before. To list all possibilities of `[dataset]` simply run the
script without parameters.

## Where's my output?
After having run `./run.sh [dataset]` (see above), the script prints some
output, which is useful to quickly validate the contents of the grid. The last
line shows the directory where all results are stored, which has the form
`dataset-date`, where `dataset` is the value given to the run script and `date`
is a numerical date when the generation was started. The date is added so runs
for the same dataset do not overwrite each other's output.

The contents of this directory are:

* `DATASET`: The directory created by `mg5_aMC`. A few interesting files in
  this subdirectory are:
  * `Events/*/MADatNLO.HwU`: histograms with uncertainties (HwU)
  * `Events/*/amcblast_obs_*.root`: grids created by `mg5_aMC`, not yet merged
    together
* `DATASET.{pineappl,root}`: All grids created by `mg5_aMC` merged together.
* `launch.log`: Output of `mg5_aMC` during the 'launch' phase
* `launch.txt`: Run card for the 'launch' phase, with all variables substituted
  to their final values
* `output.log`: Output of `mg5_aMC` during the 'output' phase
* `output.txt`: Run card for the 'output' phase, with all variables substituted
  to their final values
* `pineappl.convolute`: Output of `pineappl convolute`
* `pineappl.orders`: Output of `pineappl orders`
* `pineappl.pdf_uncertainty`: Output of `pineappl pdf_uncertainty`
* `results.log`: The numerical results of the run, comparing the results of the
  grid against the results from `mg5_aMC`
* `time.log`: Total `time` needed for the run
