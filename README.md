# Runcards for NNPDF
This repository stores all runcards needed to generate PineAPPL grids for the
processes included in NNPDF.

## How do I generate a PineAPPL grid for an existing dataset?
Run

    ./run.sh

to get a list available datasets. Pick the one you want to generate, and pass
it to `./run.sh`. The following can be used to test the toolchain and should
run fairly quickly:

    ./run.sh TEST_RUN_SH

If any software ([Madgraph5_aMC@NLO](https://launchpad.net/mg5amcnlo),
[PineAPPL](https://github.com/N3PDF/pineappl)) is missing, `./run.sh` will
attempt to install it; this works independently of the chosen dataset.

## How do I implement a new dataset?
The following files are important for each data set; they must be in the folder
`nnpdf31_proc/DATASET`, where `DATASET` is the NNPDF identifier for the
dataset.

* The `output.txt` file (compulsory). This file contains the instructions to
  generate the source code for the relevant process. For details, please see
  [arXiv:1804.10017](http://arxiv.org/abs/arXiv:1804.10017) and
  [arXiv:1405.0301](http://arxiv.org/abs/arXiv:1405.0301). The variable
  `@OUTPUT@` must be used to generate the directory containing the source
  files.

* The `launch.txt` file (compulsory). This file contains the instructions to
  run the relevant process, including the relevant physical parameters and
  cuts. Since the parameter values are inserted by `run.sh`, do not insert the
  numerical values into the text file but rather the run variables. Supported
  are `@GF`, `@MH@`, `@MT@`, `@MW@`, `@MZ@`, `@WH@`, `@WT@`, `@WW@`, and
  `@WZ@`. The names are the same as chosen by `mg5_aMC`, but written in
  uppercase and surrounded with `@`. For details about more parameters, please
  see the `Template/NLO/Cards/run_card.dat` file in Madgraph5_aMC@NLO.

* The `analysis.f` file (compulsory). This Fortran file must fill the
  histograms from which the `HwU` files (histograms with uncertainties) and the
  PineAPPL grids are generated. Note that a single histogram must not contain
  more than 100 bins, otherwise Madgraph5_aMC@NLO will crash. However, big
  histograms can be split up into multiple histograms, for which `run.sh` will
  merge the PineAPPL grids together.

* The `*.patch` file(s) (optional). These are one or more `.patch` files that
  are applied after Madgraph5_aMC@NLO has generated the sources. For instance, to
  use a dynamical scale, a patch modifying `setscales.f` file should be
  included in the directory. To create patches use the command `diff -Naurb
  original new > patch.patch`. The patches are applied in an unspecified order,
  using `patch -p1 ...`.

* The `postrun.sh` file (optional, must be executable). This is a BASH script
  which is run after the successful generation of the PineAPPL grid and can be
  used to perform additional operations, such as rescaling. The environment
  variable `$GRID` contains the relative path the PineAPPL grid. Typically this
  file contains instructions to remap the one-dimensional histograms generated
  by Madgraph5_aMC@NLO into higher dimensional ones with the proper limits.

* The `metadata.txt` file (optional). This file collects all metadata, which is
  written into the grid after generation. Arbitrary `key=value` pairs are
  supported, the most common are:

  - `arxiv`: The arxiv number of the experimental analysis, or if there are
    more than one, comma-seperated numbers.
  - `description`: A short description of the process/observables. Make sure to
    include the name of the experiment and the centre-of-mass energy.
  - `hepdata`: The DOI pointing to the experimental data, or a comma-separated
    list of DOIs. Preferably this points to specific tables of the observables
    specified below, as the hepdata entries usually show many of them.
  - `x1_label`: The name of the first (`x2` = second, `x3` = third, ...)
    observable.
  - `x1_label_tex`: The name of the observable, written in LaTeX.
  - `x1_unit`: The unit of the observable (typically `GeV`). If this key is not
    present, the corresponding observable is assumed to be dimensionless.
  - `y_label`: The label for the differential cross section.
  - `y_label_tex`: The label for the differential cross section, written in
    LaTeX. Use `\frac{a}{b}` instead of `a/b` for fractions.
  - `y_unit`: The unit for the cross section (typically `pb` for dimensionless
    observables, or `pb/GeV` or `pb/GeV^2`).

  This key-value pairs are written into the final PineAPPL, to allow the user
  to easily identify what is stored in the grid and how it was generated. It
  also allows for easily plotting the contents of the grids.

  In addition to the data supplied above, the script `run.sh` and PineAPPL will
  automatically add the following metadata:

  - `initial_state_{1,2}`: The hadronic initial states of the grid, given as
    PDG ids, typically `2212` for protons, `-2212` for anti-protons, and so on.
  - `mg5amc_repo` and `mg5amc_revno`: The repository and revision number of the
    Madgraph5_aMC@NLO version that was used to generate the grid.
  - `pineappl_gitversion`: The PineAPPL version that was used to generate the
    grid.
  - `results`: The comparison of the HwU results against a convolution of the
    PineAPPL grid with the PDF selected in `launch.txt`. This is the same table
    printed at the end by `run.sh`, and is used to verify the contents of each
    grid. It also stores the MC uncertainties.
  - `runcard`: Madgraph5_aMC@NLO's runcard that was used to generate the grid.
    Here all parameters are documented.
  - `runcard_gitversion`: The git version of this repository that was used to
    generate the grid.

## What is all the output?
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
