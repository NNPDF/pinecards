Generate a PineAPPL grid
========================

To generate a PineAPPL grid run:

.. code-block:: sh

   pinefarm run <RUNCARD> <THEORY>

In order to get a list of available pinecards run:

.. code-block:: sh

   pinefarm list pinecards

.. note::

   Use `TEST_RUN_SH` in order to test the toolchain, it should run fairly
   quickly.

Analogously for theories:

.. code-block:: sh

   pinefarm list theories

If any software is missing, it will be installed on the fly, including:

- `Madgraph5_aMC@NLO <https://launchpad.net/mg5amcnlo>`_
- `PineAPPL <https://github.com/N3PDF/pineappl>`_

.. note::

   Only the code relevant to run the selected pinecard will be installed.
