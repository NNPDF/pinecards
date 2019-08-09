# aMCblast
The madgraph/aMCblast project consists of a set of tools for the automated
generation of applgrids that include NLO(QCD+EW) corrections.

## Prerequisites
In order to successfully generate an applgrid, the following packages are
required:

* the modfied version of **APPLgrid** available in the `applgridphoton` folder;
* the modified version of **Madgraph/MC@NLO** *https://code.launchpad.net/~amcblast/+junk/3.0.2* installable as `bzr branch lp:~amcblast/+junk/3.0.2` (bazaar is
available from http://bazaar.canonical.com/en/ if it's not on you rmachine).

## Workflow
After a successful installation of the prerequisites above, one should prepare
the following files to generate an applgrid.

* the *output* file.