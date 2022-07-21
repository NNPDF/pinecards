# -*- coding: utf-8 -*-
"""
    Runner for vrap producing pineappl grids

    Uses a modified version of vrap https://github.com/NNPDF/hawaiian_vrap
    which produces pineappl grids. It has been tested for FT DY kinematics

    A ``vrap`` dataset includes a single ``vrap.yaml`` file which defines
    how ``vrap`` will be run.
    A dataset can include many kinematic files (min. 1), ``vrap`` will be run
    once per kinematic file.
    Vrap datatasets can also include ``cfactors`` which need to match the name
    of the kinematic files and will be applied to the corresponding run
    ex: if the kinematic file is call "906_bin0.dat" the corresponding cfactors
        are "ACC_906_bin0.dat" and "QCD_906_bin0.dat"
"""
import subprocess as sp
import tempfile

import numpy as np
import pandas as pd
import yaml
from ekobox import genpdf
from lhapdf_management import environment
from pineappl.bin import BinRemapper
from pineappl.grid import Grid

from .. import configs, install
from . import interface

_PINEAPPL = "test.pineappl.lz4"
VERSION = "1.2"
_POSITIVITY_PDFS = {
    "pos_ddb": [1, -1, 21],
    "pos_uub": [2, -2, 21],
    "pos_ssb": [3, -3, 21],
}


def is_vrap(name):
    """
    Checks whether this is a dataset to be run with vrap
    """
    return (configs.configs["paths"]["runcards"] / name / "vrap.yaml").exists()


def yaml_to_vrapcard(yaml_dict, pdf, output_file):
    """
    Converts the dictionary from `vrap.yaml` file into a vrap runcard
    """
    input_yaml = dict(yaml_dict)
    # Load the run-specific options
    input_yaml["PDFfile"] = f"{pdf}.LHgrid"
    # Remove possible spurious options
    if "positivity_pdf" in input_yaml:
        input_yaml.pop("positivity_pdf")

    as_lines = [f"{k} {v}" for k, v in input_yaml.items()]
    output_file.write_text("\n".join(as_lines))


def gen_pos_pdf(pdfname, base_pdf="NNPDF40_nnlo_as_01180"):
    """
    Generate ``pdfname`` according to the rules in _POSITIVITY_PDFS
    """
    # If the pdfname does not exist, generate it
    if not (environment.datapath / pdfname).exists():
        pdflabels = _POSITIVITY_PDFS[pdfname]
        genpdf.generate_pdf(pdfname, pdflabels, install=True, parent_pdf_set=base_pdf)


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

        # Check whether there are cfactors to be applied
        self._cfactors = None
        if (self.source / "cfactors").exists():
            self._cfactors = []
            cfac_path = self.source / "cfactors"
            for kinfile in self._kin_cards:
                # Read the cfactors
                tmp = []
                for cfac_file in cfac_path.glob(f"*{kinfile.name}"):
                    cf, _ = np.loadtxt(cfac_file, skiprows=9, unpack=True)
                    tmp.append(cf)
                self._cfactors.append(tmp)

        # Write the input card in the vrap format
        self._input_card = (self.dest / self.name).with_suffix(".dat")
        yaml_dict = yaml.safe_load(input_card.open("r", encoding="utf-8"))

        # If this is a positivity runcard, generate the fake pdf
        if "positivity_pdf" in yaml_dict:
            pdfname = yaml_dict["positivity_pdf"]
            gen_pos_pdf(pdfname)
            self.pdf = pdfname

        yaml_to_vrapcard(yaml_dict, self.pdf, self._input_card)

        self._partial_grids = []
        self._partial_results = []

    def run(self):
        """Run vrap for the given runcards
        After running vrap, the resulting grid will be optimized, cfactors
        (for instance, ACCEPTANCE factors) applied.
        The MC results for each run (writen to results.out) will be read
        """
        for b, kin_card in enumerate(self._kin_cards):
            sp.run(
                [configs.configs["commands"]["vrap"], self._input_card, kin_card],
                cwd=self.dest,
                check=True,
            )
            tmppine = self.dest / _PINEAPPL

            # Now change the name of the grid, optimize and apply any necessary cfactors
            grid = Grid.read(tmppine.as_posix())

            # And give it a sensible name
            if len(self._kin_cards) == 1:
                pinename = self.grid
            else:
                pinename = self.dest / f"{self.name}_bin_{b}.pineappl.lz4"

            # Read the MC results for later comparison
            _, _, cv, stat = np.loadtxt(self.dest / "results.out", unpack=True)

            # Apply cfactors if necessary
            if self._cfactors is not None:
                cfs = self._cfactors[b]
                for cf in cfs:
                    cv *= cf
                    stat *= cf
                    grid.scale_by_bin(cf.flatten())

            # Now optimize the grid
            grid.optimize()
            grid.write(pinename)
            tmppine.unlink()

            self._partial_grids.append(pinename)

            # Apply cfactors if needed
            self._partial_results.append((cv, stat))

    def generate_pineappl(self):
        """If the run contain more than one grid, merge them all"""
        if len(self._partial_grids) > 1:
            # Use the first subgrid as main grid
            main_grid = Grid.read(self._partial_grids[0].as_posix())
            n = len(main_grid.bin_left(0))
            rebin = BinRemapper(np.ones(n), [(i, i) for i in range(n)])
            main_grid.set_remapper(rebin)
            with tempfile.TemporaryDirectory() as tmp:
                for i, grid_path in enumerate(self._partial_grids[1:]):
                    tmp_output = f"{tmp}/bin_{i}.pineappl.lz4"
                    tmp_grid = Grid.read(grid_path.as_posix())
                    tmp_grid.set_remapper(rebin)
                    tmp_grid.write(tmp_output)
                    # Now merge it into the main grid!
                    main_grid.merge_from_file(tmp_output)
            main_grid.write(self.grid)

    def results(self):
        """Combines the results of the partial runs of vrap
        in order to compare with the generated grid
        """
        cv, stat_errors = zip(*self._partial_results)
        final_cv = np.sum(cv, axis=0)
        final_stat = np.sqrt(np.sum(np.power(stat_errors, 2), axis=0))

        d = {
            "result": final_cv,
            "error": final_stat,
            "sv_min": np.zeros_like(final_cv),
            "sv_max": np.zeros_like(final_cv),
        }

        return pd.DataFrame(data=d)

    def collect_versions(self):
        """Currently the version is defined by this file"""
        vrap_run = sp.run(
            [configs.configs["commands"]["vrap"], "--version"],
            capture_output=True,
            check=True,
        )
        vrap_version = vrap_run.stdout.decode().split()[-1]
        return {"vrap_version": vrap_version}

    @staticmethod
    def install():
        install.hawaiian_vrap()
