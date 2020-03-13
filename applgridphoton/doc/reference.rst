Reference
=========

Structures
----------

.. doxygenstruct:: pineappl_lumi
.. doxygenstruct:: pineappl_grid
.. doxygenstruct:: pineappl_storage

Lumi functions
--------------

.. doxygenfunction:: pineappl_lumi_new
.. doxygenfunction:: pineappl_lumi_delete
.. doxygenfunction:: pineappl_lumi_add

Storage functions
-----------------

.. doxygenfunction:: pineappl_storage_new
.. doxygenfunction:: pineappl_storage_delete
.. doxygenfunction:: pineappl_storage_get_bool
.. doxygenfunction:: pineappl_storage_get_double
.. doxygenfunction:: pineappl_storage_get_int
.. doxygenfunction:: pineappl_storage_get_string
.. doxygenfunction:: pineappl_storage_set_bool
.. doxygenfunction:: pineappl_storage_set_double
.. doxygenfunction:: pineappl_storage_set_int
.. doxygenfunction:: pineappl_storage_set_string

Grid creation and deletion functions
------------------------------------

.. doxygenfunction:: pineappl_grid_new
.. doxygenfunction:: pineappl_grid_read
.. doxygenenum:: pineappl_subgrid_format
.. doxygenfunction:: pineappl_grid_delete

Grid getters
------------

.. doxygenfunction:: pineappl_grid_get_subgrids
.. doxygenfunction:: pineappl_grid_get_subgrid_format
.. doxygenfunction:: pineappl_grid_get_subgrid_params

Grid filling functions
----------------------

.. doxygenfunction:: pineappl_grid_fill

Grid helper functions
---------------------

.. doxygenfunction:: pineappl_grid_scale
.. doxygenfunction:: pineappl_grid_write

Convolution with PDFs
---------------------

.. doxygenfunction:: pineappl_grid_convolute
.. doxygentypedef:: pineappl_func_xfx
.. doxygentypedef:: pineappl_func_alphas
