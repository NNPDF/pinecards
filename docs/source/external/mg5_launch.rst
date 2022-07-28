Launch
======

Here is briefly recapped |mg5|'s '``launch.txt`` syntax and described the
special syntax that is allowed for (and introduced by) pinecards.

Relevant native syntax
----------------------

Special syntax
--------------

Since the parameter values are inserted by ``run.sh``, do not insert the
numerical values into the text file but rather the run variables. Supported are
``@GF``, ``@MH@``, ``@MT@``, ``@MW@``, ``@MZ@``, ``@WH@``, ``@WT@``, ``@WW@``,
and ``@WZ@``. The names are the same as chosen by ``mg5_aMC``, but written in
uppercase and surrounded with ``@``. For details about more parameters, please
see the ``Template/NLO/Cards/run_card.dat`` file in |mg5|.
