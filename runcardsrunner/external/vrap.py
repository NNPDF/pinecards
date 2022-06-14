# -*- coding: utf-8 -*-
"""
    Runner for vrap producing pineappl grids

    Uses a modified version of pineappl https://github.com/NNPDF/hawaiian_vrap
    which produces pineappl grids. It has been tested for FT DY kinematics
"""
import subprocess as sp

import numpy as np
import pandas as pd
import yaml
from pineappl.grid import Grid

from .. import install, paths
from . import interface

_PINEAPPL = "test.pineappl.lz4"
VERSION = 1.0


def is_vrap(name):
    """
    Checks whether this is a dataset to be run with vrap
    """
    return (paths.runcards / name / "vrap.yaml").exists()


def yaml_to_vrapcard(yaml_file, pdf, output_file):
    """
    Converts a `vrap.yaml` file into a vrap runcard
    """
    input_yaml = yaml.safe_load(yaml_file.open("r", encoding="utf-8"))
    # Load the run-specific options
    input_yaml["PDFfile"] = f"{pdf}.LHgrid"
    as_lines = [f"{k} {v}" for k, v in input_yaml.items()]
    output_file.write_text("\n".join(as_lines))


class Vrap(interface.External):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        # For FTDY due to the strange acceptance cuts
        # we can have many kinematics, therefore:
        kin_cards = list(self.source.glob(f"{self.name}*.dat"))
        if not kin_cards:
            raise FileNotFoundError(f"No kinematic cards found for {self.name}")
        self._kin_cards = sorted(kin_cards)

        input_card = self.source / "vrap.yaml"
        if not input_card.exists():
            raise FileNotFoundError(
                f"No vrap input card found for {self.name}: {input_card}"
            )

        # Write the input card in the vrap format
        self._input_card = (self.dest / self.name).with_suffix(".dat")
        yaml_to_vrapcard(input_card, self.pdf, self._input_card)

        self._subgrids = []

    def run(self):
        """Run vrap for the given runcards"""
        for b, kin_card in enumerate(self._kin_cards):
            sp.run(
                [paths.vrap_exe, self._input_card, kin_card], cwd=self.dest, check=True
            )
            # Now optimize the grid
            tmppine = self.dest / _PINEAPPL
            grid = Grid.read(tmppine.as_posix())
            grid.optimize()

            # And give it a sensible name
            if len(self._kin_cards) == 1:
                pinename = self.grid
            else:
                pinename = self.source / f"{self.name}_bin_{b}.pineappl.lz4"

            grid.write(pinename)
            tmppine.unlink()

    def generate_pineappl(self):
        pass

    def results(self):
        """Loads the results as reported by vrap in results.out"""
        _, _, cv = np.loadtxt(self.dest / "results.out", unpack=True)

        d = {
            "result": cv,
            "error": np.zeros_like(cv),
            "sv_min": np.zeros_like(cv),
            "sv_max": np.zeros_like(cv),
        }
        return pd.DataFrame(data=d)

    def collect_versions(self):
        """Currently the version is defined by this file"""
        return {"version": str(VERSION)}

    @staticmethod
    def install():
        install.hawaiian_vrap()
