Metadata
========

This file collects all metadata, which is written into the grid after
generation.

Arbitrary ``key=value`` pairs are supported, the most common are:

- ``arxiv``: The arxiv number of the experimental analysis, or if there are
  more than one, comma-seperated numbers.
- ``description``: A short description of the process/observables. Make sure to
  include the name of the experiment and the centre-of-mass energy.
- ``hepdata``: The DOI pointing to the experimental data, or a comma-separated
  list of DOIs. Preferably this points to specific tables of the observables
  specified below, as the hepdata entries usually show many of them.
- ``nnpdf_id``: The NNPDF ID, which can denote multiple datasets.
- ``x1_label``: The name of the first (``x2`` = second, ``x3`` = third, ...)
  observable.
- ``x1_label_tex``: The name of the observable, written in LaTeX.
- ``x1_unit``: The unit of the observable (typically ``GeV``). If this key is not
  present, the corresponding observable is assumed to be dimensionless.
- ``y_label``: The label for the differential cross section.
- ``y_label_tex``: The label for the differential cross section, written in
  LaTeX. Use ``\frac{a}{b}`` instead of ``a/b`` for fractions.
- ``y_unit``: The unit for the cross section (typically ``pb`` for dimensionless
  observables, or ``pb/GeV`` or ``pb/GeV^2``).

This key-value pairs are written into the final PineAPPL, to allow the user
to easily identify what is stored in the grid and how it was generated. It
also allows for easily plotting the contents of the grids.

.. note::

   Further metadata are specific for each external, you can find them in the
   respective external page, in the section "Additional metadata".
