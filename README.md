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
After a successful installation of the prerequisites above, one should prepare
the following files to generate an applgrid. These files are located in the
eponymous subdirectories of the `nnpdf31_proc` folder and should be named
after the corresponding data set included in a NNPDF fit.

* The *output* file (compulsory). This is a `.txt` file containing the 
  instructions to generate the relevant process. For details, please see
  [arXiv:1804.10017](http://arxiv.org/abs/arXiv:1804.10017) and 
  [ arXiv:1405.0301](http://arxiv.org/abs/arXiv:1405.0301).

* The *launch* file (compulsory). This is a `.txt` file containing the 
  instructions to run the relevant process, including any values of the 
  relevant physical parameters.

* The *analysis* file(s) (compulsory). These are one or more `.f` files
  containing the instructions for the kinematic analysis in the `HwU` format. 
  Because of memory issues, one needs to wirte one different file for each 
  kinematic bin so far. Examples on how to do so can be found in the
  'Template/NLO/FixedOrderAnalysis/` folder of Madgraph/MC@NLO.

* The patch(es) file(s) (optional). These are one or more `.f` files
  containing any patches to the default Madgraph/MC@NLO. For instance should
  you define a dynamical scale, the modified `setscales.f` file whould be 
  amended and stored there. The list of patches, with a description
  including the data set to which they apply to, should be kept updated in the
  `patches.txt` file.

Provided the above files, the production of the grids only requires the user to
run the `./run.sh [nameset] [bin]` script. The script takes two variables as an
input:

* [nameset]: the name of the data set (e.g. `CMSTTBARTOT7TEV`);
* [bin]:     the ID of the bin in the experiment (e.g. `3`, for the third bin).

Once the grids are produced, the user should rename and store them in the 
`applgrids_ew` folder following the same conventions in order for the NNPDF
`applgrids` repository.