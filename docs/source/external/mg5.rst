Mg5aMC\@NLO
===========

Runcard structure
-----------------

- The ``output.txt`` file (compulsory). This file contains the instructions to
  generate the source code for the relevant process. For details, please see
  `arXiv:1804.10017 <http://arxiv.org/abs/arXiv:1804.10017>`_ and
  `arXiv:1405.0301 <http://arxiv.org/abs/arXiv:1405.0301>`_. The variable
  ``@OUTPUT@`` must be used to generate the directory containing the source
  files.

- The ``launch.txt`` file (compulsory). This file contains the instructions to
  run the relevant process, including the relevant physical parameters and cuts.
  Since the parameter values are inserted by ``run.sh``, do not insert the
  numerical values into the text file but rather the run variables. Supported
  are ``@GF``, ``@MH@``, ``@MT@``, ``@MW@``, ``@MZ@``, ``@WH@``, ``@WT@``,
  ``@WW@``, and ``@WZ@``. The names are the same as chosen by ``mg5_aMC``, but
  written in uppercase and surrounded with ``@``. For details about more
  parameters, please see the ``Template/NLO/Cards/run_card.dat`` file in
  Madgraph5_aMC\@NLO.

- The ``analysis.f`` file (compulsory). This Fortran file must fill the
  histograms from which the ``HwU`` files (histograms with uncertainties) and
  the PineAPPL grids are generated. Note that a single histogram must not
  contain more than 100 bins, otherwise Madgraph5_aMC\@NLO will crash. However,
  big histograms can be split up into multiple histograms, for which `run.sh`
  will merge the PineAPPL grids together.

- The ``*.patch`` file(s) (optional). These are one or more ``.patch`` files
  that are applied after Madgraph5_aMC\@NLO has generated the sources. For
  instance, to use a dynamical scale, a patch modifying ``setscales.f`` file
  should be included in the directory. To create patches use the command ``diff
  -Naurb original new > patch.patch``. The patches are applied in an unspecified
  order, using ``patch -p1 ...``.

Output
------

- ``DATASET``: The directory created by ``mg5_aMC``. A few interesting files in
  this subdirectory are:

  - ``Events/-/MADatNLO.HwU``: histograms with uncertainties (HwU)
  - ``Events/-/amcblast_obs_-.pineappl``: grids created by ``mg5_aMC``, not yet
    merged together

- ``output.txt``: Run card for the 'output' phase, with all variables substituted
  to their final values
- ``output.log``: Output of the external runner during the 'output' phase
- ``launch.txt``: Run card for the 'launch' phase, with all variables substituted
  to their final values
- ``launch.log``: Output of the external runner during the 'launch' phase
- ``pineappl.convolute``: Output of ``pineappl convolute``
- ``pineappl.orders``: Output of ``pineappl orders``
- ``pineappl.pdf_uncertainty``: Output of ``pineappl pdf_uncertainty``
