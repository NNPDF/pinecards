``rr`` command line interface
=============================

This is not a replacement of the CLI help, you can get it with:

.. code-block:: sh

   rr --help # ./rr --help

It is just a brief recap of the subcommands and their goal.

``install``
-----------

This is only available in the executable script provided in the repository, and
it is used for bootstrapping the development environment.

It is not powered by the ``runcardsrunner`` package.

``run``
-------

It is the main command provided, and it runs the specified runcard in the
context of the selected theory.

The output will be stored in a directory in the current path, with the name
``<RUNCARD>-<YYYYMMDDhhmmss>``, where:

- the first part is the name of the selected runcard (that is also the name of
  the folder in which all the files are stored)
- the second part is the timestamp of the moment in which the command is issued

``update``
----------

Update the metadata of the specified grid, with the content of the
``metadata.txt`` file in the current version of the runcard.

``merge``
---------

Merge the specified grids' content into a new grid.
