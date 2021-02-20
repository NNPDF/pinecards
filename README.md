# Runcards for NNPDF
This repository stores all runcards needed to generate PineAPPL grids for the
processes included in NNPDF.

## How do I generate a PineAPPL grid for an existing analysis?
Run

    ./run.sh

to get a list available anaylses. Pick the one you're interested in, and pass
it to `./run.sh`. The following should run very quickly:

    ./run.sh TEST_RUN_SH

If any software ([Madgraph5_aMC@NLO](https://launchpad.net/mg5amcnlo),
[PineAPPL](https://github.com/N3PDF/pineappl)) is missing, `./run.sh` will
attempt to install it; this works independently of the chosen dataset.

## How do I implement a new dataset?
The following files are important for each data set; they must be in the folder
`nnpdf31_proc/DATASET`, where `DATASET` is the NNPDF identifier for the
dataset.

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
  ones are `@MZ@`, and `@MW@`. The names are the same as chosen by `mg5_aMC`,
  but written in uppercase and surrounded with `@`. For details about more
  parameters, please see the `Template/NLO/Cards/run_card.dat` file in
  Madgraph/MC@NLO.

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
  - `x1_label`: The name of the first (second, third, ...) observable.
  - `x1_label_tex`: The name of the observable, written in LaTeX.
  - `x1_unit`: The unit of the observable (typically `GeV`). If this key is not
    present, the corresponding observable is assumed to be dimensionless.
  - `y_label`: The unit for the cross section (typically `pb`).

## Where's my output?
After having run `./run.sh DATASET` (see above), the script prints a table,
which is useful to quickly validate the MC uncertainty and the interpolation
error of PineAPPL. The last line shows the directory where all results are
stored, which has the form `DATASET-DATE`, where `DATASET` is the value given
to the run script and `DATE` is a numerical date when the generation was
started. The date is added so runs for the same dataset do not overwrite each
other's output.

The most important file in the output directory is

    DATASET-DATE/DATASET.pineappl.lz4

which is the PineAPPL grid.

The remaining contents of this directory are useful for testing and debugging:

* `DATASET`: The directory created by `mg5_aMC`. A few interesting files in
  this subdirectory are:
  * `Events/*/MADatNLO.HwU`: histograms with uncertainties (HwU)
  * `Events/*/amcblast_obs_*.pineappl`: grids created by `mg5_aMC`, not yet
    merged together
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
  grid against the results from `mg5_aMC`. The first column (PineAPPL) are the
  interpolated results, which should be similar to the Monte Carlo (MC) results
  in the second column. The third column gives the relative MC uncertainty
  (sigma). The next column gives the differences in terms of sigma. The final
  three columns give the per mille differences of the central, minimum, and
  maximum scale varied results. Ideally the first two columns are the same and
  the remaining columns are zero.
* `time.log`: Total `time` needed for the run
