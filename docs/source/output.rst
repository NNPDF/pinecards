What is all the output?
=======================

After having run ``pinefarm run DATASET THEORY`` (see :doc:`cli`), the script
prints a table, which is useful to quickly validate the MC uncertainty and the
interpolation error of PineAPPL. The last line shows the directory where all
results are stored, which has the form ``DATASET-DATE``, where ``DATASET`` is
the value given to the run script and ``DATE`` is a numerical date when the
generation was started. The date is added so runs for the same dataset do not
overwrite each other's output.

The most important file in the output directory is

    ``DATASET-DATE/DATASET.pineappl.lz4``

which is the PineAPPL grid.

The remaining contents of this directory are useful for testing and debugging:

- ``results.log``: The numerical results of the run, comparing the results of the
  grid against the results from ``mg5_aMC``. The first column (PineAPPL) are the
  interpolated results, which should be similar to the Monte Carlo (MC) results
  in the second column. The third column gives the relative MC uncertainty
  (sigma). The next column gives the differences in terms of sigma. The final
  three columns give the per mille differences of the central, minimum, and
  maximum scale varied results. Ideally the first two columns are the same and
  the remaining columns are zero.
- ``time.log``: Total ``time`` needed for the run

.. note::

  The resulting PineAPPL grid will contain the metadata written in the
  ``metadata.txt`` file.

  In addition , the script ``run.sh`` and PineAPPL will automatically add the
  following metadata:

  - ``initial_state_{1,2}``: The hadronic initial states of the grid, given as
    PDG ids, typically ``2212`` for protons, ``-2212`` for anti-protons, and so on.
  - ``lumi_id_types``: The meaning of the luminosities IDs in the definition of
    the luminosity function of a PineAPPL grid. This is set to ``pdg_mc_ids`` to
    signal they are PDG ids (with a possible exception of the gluon, for which
    ``0`` may be used).
  - ``mg5amc_repo`` and ``mg5amc_revno`` (only :doc:`external/mg5` grids): The
    repository and revision number of the Madgraph5_aMC\@NLO version that was
    used to generate the grid.
  - ``pineappl_gitversion``: The PineAPPL version that was used to generate the
    grid.
  - ``results``: The comparison of the HwU results against a convolution of the
    PineAPPL grid with the PDF selected in ``launch.txt``. This is the same table
    printed at the end by ``run.sh``, and is used to verify the contents of each
    grid. It also stores the MC uncertainties.
  - ``pinecard``: Madgraph5_aMC\@NLO's pinecard that was used to generate the grid.
    Here all parameters are documented.
  - ``pinecard_gitversion``: The git version of this repository that was used to
    generate the grid.
  - ``yadism_version`` (only :doc:`external/yadism` grids): The |yadism| version
    used to generate the grid (if not a released version have been used it
    includes also git details).

Runner dependent output
-----------------------

Part of the output is specific to the selected runner, and described in the
corresponding :doc:`section <external/index>`.
