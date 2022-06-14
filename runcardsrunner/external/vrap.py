# -*- coding: utf-8 -*-
"""
vrap blablabla
"""
import subprocess as sp

from .. import install, paths
from . import interface

_PINEAPPL = "test.pineappl.lz4"


def is_vrap(name):
    """
    Checks whether this is a dataset to be run with vrap
    """
    return name.startswith("DYE")


class Vrap(interface.External):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        # For FTDY due to the strange acceptance cuts
        # we can have many kinematics, therefore:
        kin_cards = list(self.source.glob(f"{self.name}*.dat"))
        if not kin_cards:
            raise FileNotFoundError(f"No kinematic cards found for {self.name}")

        order = "nlo"  # for now there are no other options
        input_card = self.source / f"input{self.name}{order}.dat"
        if not input_card.exists():
            raise FileNotFoundError(
                f"No vrap input card found for {self.name}: {input_card}"
            )

        self._kin_cards = sorted(kin_cards)
        self._input_card = input_card
        self._subgrids = []

    def run(self):
        """Run vrap for the given runcards"""
        target = "deut" if "deut" in self._input_card.as_posix() else ""
        for b, kin_card in enumerate(self._kin_cards):
            sp.run(
                [paths.vrap_exe, self._input_card, kin_card], cwd=self.dest, check=True
            )
            pinename = f"{self.name}{target}_bin_{b}.pineappl.lz4"
            sp.run(
                ["pineappl", "optimize", _PINEAPPL, pinename], cwd=self.dest, check=True
            )
            self._subgrids.append(self.dest / pinename)

    def generate_pineappl(self):
        """vrap generates all pineappl grid already at the ``run`` stage"""
        # Remove possible intermediate pineappls
        tmppine = self.dest / _PINEAPPL
        if tmppine.exists():
            tmppine.unlink()
        # If we have one single grid, remove the bin index
        if len(self._kin_cards) == 1:
            self._subgrids[0].rename(self.grid)

    def results(self):
        import numpy as np
        import pandas as pd

        d = {
            "result": np.random.rand(2),
            "error": np.random.rand(2),
            "sv_min": np.random.rand(2),
            "sv_max": np.random.rand(2),
        }
        return pd.DataFrame(data=d)

    def collect_versions(self):
        return {}

    @staticmethod
    def install():
        install.hawaiian_vrap()
