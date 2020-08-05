Luminosity function
===================

Since PineAPPL grids store integrated (or differential) cross sections in a PDF-independent way, we
need some description of how the convolutation with PDFs is to be done later. This requires
enumerating the partonic initial states that contribute to the cross section. Often one wants to
reuse the matrix elements for different initial states because they are the same or differ by a
trivial factor (usually, but not limited to Cabibbo--Kobayashi--Maskawa matrix elements). In that
case the factors have to be specified as well.

For Drell--Yan muon-pair production, for example, this is done in the following way (in C or C++):

.. code-block:: cpp
   :linenos:

   int pdg_ids[6];
   double factors[3];

   // create a new luminosity function
   pineappl_lumi* lumi = pineappl_lumi_new();

   pdg_ids = { 1, -1, 3, -3, 5, -5 };
   factors = { 2.0, 3.0, 4.0 };
   pineappl_lumi_add(lumi, 3, pdg_ids, factors);

   pdg_ids = { 2, -2, 4, -4 };
   factors = { 1.0, 1.0 };
   pineappl_lumi_add(lumi, 2, pdg_ids, factors);

   // add more initial-state combinations

   // use the lumi function to create a grid

   pineappl_lumi_delete(lumi);

The first call to :cpp:func:`pineappl_lumi_new` returns a new handle that will represent the
luminosity function that we are about to create. The next two calls to :cpp:func:`pineappl_lumi_add`
each add an entry to the luminosity function, using the PDG ids for each initial state. Note that
order of the calls make a difference; it must correspond to the order of ``weights`` in the call to
:cpp:func:`pineappl_grid_fill`. When later convoluted with PDFs, the program above will perform the
convolution as follows:

.. math::

   \sigma = &\left( 2 \cdot f_{\mathrm{d}} f_{\bar{\mathrm{d}}} +
                    3 \cdot f_{\mathrm{s}} f_{\bar{\mathrm{s}}} +
                    4 \cdot f_{\mathrm{b}} f_{\bar{\mathrm{b}}} \right) \otimes w_0 + \\
            &\left( 1 \cdot f_{\mathrm{u}} f_{\bar{\mathrm{u}}} +
                    2 \cdot f_{\mathrm{c}} f_{\bar{\mathrm{c}}} \right) \otimes w_1 +
            \ldots

where :math:`f_a f_b = f_a (x_1, Q^2) f_b (x_2, Q^2)` are the PDFs and :math:`w_i = w_i(x_1, x_2,
Q^2)` are functions which are interpolated using the i-th entry of the ``weights`` parameter that
was used to the fill the grid with :cpp:func:`pineappl_grid_fill`.
