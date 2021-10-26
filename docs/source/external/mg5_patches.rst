Patches
=======

Patches are ``bla bla bla`` available in :doc:`launch files <mg5_launch>`.

For instance, to use a dynamical scale, a patch modifying ``setscales.f`` file
should be included in the directory. To create patches use the command ``diff
-Naurb original new > patch.patch``. The patches are applied in an unspecified
order, using ``patch -p1 ...``.

List of available patches
-------------------------

``change_etaj_to_rapj``
~~~~~~~~~~~~~~~~~~~~~~~

``no_pole_cancellation_checks``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``set_tau_min``
~~~~~~~~~~~~~~~
