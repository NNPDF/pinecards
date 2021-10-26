Mg5aMC\@NLO
===========

.. toctree::
   :maxdepth: 1
   :caption: Contents:

   mg5_launch
   mg5_cuts
   mg5_patches


Runcard structure
-----------------

- The ``output.txt`` file (compulsory). This file contains the instructions to
  generate the source code for the relevant process. For details, please see
  `arXiv:1804.10017 <http://arxiv.org/abs/arXiv:1804.10017>`_ and
  `arXiv:1405.0301 <http://arxiv.org/abs/arXiv:1405.0301>`_. The variable
  ``@OUTPUT@`` must be used to generate the directory containing the source
  files.

- The ``launch.txt`` file (compulsory). This file contains the instructions to
  run the relevant process, including the relevant physical parameters and cuts,
  more info in :doc:`mg5_launch`.

- The ``analysis.f`` file (compulsory). This Fortran file must fill the
  histograms from which the ``HwU`` files (histograms with uncertainties) and
  the PineAPPL grids are generated. Note that a single histogram must not
  contain more than 100 bins, otherwise |mg5| will crash. However,
  big histograms can be split up into multiple histograms, for which the runner
  will merge the PineAPPL grids together.

- The ``*.patch`` file(s) (optional). These are one or more ``.patch`` files
  that are applied after |mg5| has generated the sources.

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
