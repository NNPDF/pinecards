# aMCblast
The madgraph/aMCblast project consists of a set of tools for the automated
generation of applgrids that include NLO(QCD+EW) corrections (mainly for 
NNPDF fit purposes).

## Prerequisites
In order to successfully generate an applgrid, the following packages are
required:

* the modfied version of **APPLgrid** available in the `applgridphoton` folder;
* the modified version of **Madgraph/MC@NLO** (*https://code.launchpad.net/~amcblast/+junk/3.0.2*) installable as 
`bzr branch lp:~amcblast/+junk/3.0.2` 
(bazaar is available from http://bazaar.canonical.com/en/ if it is not already 
installed on your machine).

## Workflow
After a successful installation of the prerequisites above, one must prepare
the following files to generate an applgrid. These files are located in the
subdirectories `nnpdf31_proc/DATASET` folders and, where `DATASET` should be
named after the corresponding data set included in a NNPDF fit.

* The `output.txt` file (compulsory). This file contains the instructions to
  generate the relevant process. For details, please see
  [arXiv:1804.10017](http://arxiv.org/abs/arXiv:1804.10017) and
  [arXiv:1405.0301](http://arxiv.org/abs/arXiv:1405.0301). The variable
  `@OUTPUT@` should be used to generate the directory containing the source
  files.

* The `launch.txt` file (compulsory). This file contains the instructions to
  run the relevant process, including the relevant physical parameters. Since
  the parameter values are inserted by `run.sh`, do not insert numerical values
  but rather the the relevant variables run variables. This one supported are
  `@MZ@`, `@MW@`, and `@YMT@`. The names are the same as chosen by `mg5_aMC`,
  but written in uppercase and surrounded with `@`. For details about more
  parameters, please see the `Template/NLO/Cards/run_card.dat` file in
  Madgraph/MC@NLO.

* The `analysis.f` file (compulsory). This one Fortran file containing the
  instructions for the kinematic analysis in the `HwU` format that must fill
  the APPLgrid file. Examples can be found in the
  `Template/NLO/FixedOrderAnalysis/` folder of Madgraph/MC@NLO.

* The `*.patch` file(s) (optional). These are one or more `.patch` files
  containing any changes to the default Madgraph/MC@NLO. For instance, should
  you define a dynamical scale, a patch modifying `setscales.f` file should be 
  included in the directory. To create patches use the command `diff -Naurb
  original new > patch.patch`. The patches are applied in an unspecified order,
  using `patch -p1 ...`.

Provided the above files, the production of the grids only requires the user to
run the `./run.sh [nameset]` script. The script takes as input:

* [nameset]: the name of the data set (e.g. `CMSTTBARTOT7TEV`);

To list all possibilities of `[nameset]` simply run the script without
parameters.
