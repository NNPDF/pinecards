####################################
Welcome to pinefarm's documentation!
####################################

This documentation is about the python package used to generate the PineAPPL
grids out of the proper runcards, called `pinefarm` (and its CLI ``rr``),
and it covers as well the runcards format.

Install ``rr``
==============

There are two ways of installing ``rr``, that are:

- **production**: used by *final user*, simply run

  .. code-block:: sh

     pip install pinefarm

  and then use ``rr`` as a command available in ``PATH``

- **develop**: used by the tools developer, bootstrap with

  .. code-block:: sh

     ./rr install

  and in this case, ``rr`` to be used is always the one provided as executable.
  in the repository.

Non Python dependencies
-----------------------

Even if the bootstrap script and the installation management try to reduce as
much as possible the amount of dependencies, still a few ingredients have to be
available on the system.

To run the CLI:

- ``python`` itself
- ``pip`` available as a module of the ``python`` that is running ``rr`` (as usually
  is)
- ``curl``

To install ``pineappl``:

- ``pkg-config``
- ``openssl.pc`` (e.g. on Debian available in the ``libssl-dev`` package)

To install ``mg5amc@nlo`` and its dependencies:

- ``gfortran``
- ``wget``


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

   API <modules/pinefarm/pinefarm>
   indices
