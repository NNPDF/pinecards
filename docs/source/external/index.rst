External runners
================

The ``pinefarm run`` is mainly a uniform interface to some Monte Carlo (and non)
generator that are able to produce PineAPPL grids.

Internally the runners are managed through a class system, with a base class
:class:`~pinefarm.external.interface.External`, that defines the basic
steps and attributes, while implementing the common actions.

Attributes:

- ``name``: name of the dataset
- ``theory``: identifier of the theory
- ``pdf``: PDF used for the comparison
- ``timestamp``, *optional*: the timestamp of the previous run, if rerunning an
  already present grid

Computed attributes:

- ``dest``: folder used for all the output
- ``source``: folder containing pinecard
- ``grid``: path of the computed grid
- ``gridtmp``: path used for auxiliary grid (removed at the end of the run)

Steps:

- :meth:`~pinefarm.external.interface.External.install`: further install
  steps, needed for the runner (not needed if the runner available as a python
  package on PyPI)
- :meth:`~pinefarm.external.interface.External.run`: compute the actual
  predictions
- :meth:`~pinefarm.external.interface.External.generate_pineappl`: collect
  predictions into a |pineappl| grid
- :meth:`~pinefarm.external.interface.External.results`: provide runner
  results on chosen PDF, for comparison with |pineappl| ``convolute`` ones
- :meth:`~pinefarm.external.interface.External.annotate_versions`: collect
  versions of all the program used to compute the results (for reproducibility)
- :meth:`~pinefarm.external.interface.External.postprocess`: apply any
  further step specified in :doc:`postprocess file <../pinecards/postrun>`, save
  :doc:`metadata <../pinecards/metadata>`, and compress the final grid
