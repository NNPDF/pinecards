##########################################
Welcome to runcardsrunner's documentation!
##########################################

This documentation is about the python package used to generate the PineAPPL
grids out of the proper runcards, called `runcardsrunner` (and its CLI ``rr``),
and it covers as well the runcards format.

Install ``rr``
==============

There are two ways of installing ``rr``, that are:

- **production**: used by *final user*, simply run

  .. code-block:: sh

     pip install runcardsrunner

  and then use ``rr`` as a command available in ``PATH``

- **develop**: used by the tools developer, bootstrap with

  .. code-block:: sh

     ./rr install

  and in this case, ``rr`` to be used is always the one provided as executable.
  in the repository.


.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Usage

   run
   output
   cli

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Runcards

   runcards/index
   runcards/metadata
   runcards/postrun

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Externals

   external/index
   external/mg5
   external/yadism

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Implementation

   API <modules/runcardsrunner/runcardsrunner>
   indices
