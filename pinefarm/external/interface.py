# -*- coding: utf-8 -*-
import abc
import os
import shutil
import subprocess

import pygit2

from .. import __version__, configs, tools


class External(abc.ABC):
    """
    Interface class for external providers.

    Parameters
    ----------
        name : str
            dataset name
        theory : dict
            theory dictionary
        pdf : str
            PDF name
        timestamp : str
            timestamp of already generated output folder

    """

    def __init__(self, name, theory, pdf, timestamp=None):
        self.name = name
        self.theory = theory
        self.pdf = pdf
        self.timestamp = timestamp

        if timestamp is None:
            self.dest = tools.create_output_folder(self.name)
        else:
            self.dest = configs.configs["paths"]["results"] / (
                self.name + "-" + self.timestamp
            )
            if not self.grid.exists():
                tools.decompress(self.grid.with_suffix(".pineappl.lz4"))

    @property
    def source(self):
        """Runcard base directory."""
        return configs.configs["paths"]["runcards"] / self.name

    @property
    def grid(self):
        """Target PineAPPL grid name."""
        return self.dest / f"{self.name}.pineappl"

    @property
    def gridtmp(self):
        """Intermediate PineAPPL grid name."""
        return self.dest / f"{self.name}.pineappl.tmp"

    def update_with_tmp(self):
        """Move intermediate grid to final position."""
        shutil.move(self.gridtmp, self.grid)

    @staticmethod
    def install():
        """Install all needed programs."""

    @abc.abstractmethod
    def run(self):
        """Execute the program."""

    @abc.abstractmethod
    def generate_pineappl(self):
        """Generate PineAPPL grid and extract output.

        Returns
        -------
        str
            output of ``pineappl convolute`` on the generate grid and selected
            :attr:`pdf`

        """

    @abc.abstractmethod
    def results(self):
        """Results as computed by the program.

        Returns
        -------
        pandas.DataFrame
            standardized dataframe with results (containing ``result``,
            ``error``, ``sv_min``, and ``sv_max`` columns)

        """

    @abc.abstractmethod
    def collect_versions(self) -> dict:
        """Collect necessary version informations.

        Returns
        -------
        dict
            program - version mapping related to programs specific to a single
            runner (common ones are already abstracted)

        """

    def annotate_versions(self):
        """Add version informations as meta data."""
        results_log = self.dest / "results.log"

        versions = self.collect_versions()
        # the pinefarm version will also pin pineappl_py version and all the
        # other python dependencies versions
        versions["pinefarm"] = __version__
        versions["pinecard"] = pygit2.Repository(
            configs.configs["paths"]["root"]
        ).describe(
            always_use_long_format=True,
            describe_strategy=pygit2.GIT_DESCRIBE_TAGS,
            dirty_suffix="-dirty",
            show_commit_oid_as_fallback=True,
        )
        # TODO: add pineappl version
        #  pineappl = configs.configs["commands"]["pineappl"]()
        versions["pineappl_capi"] = "???"

        entries = {}
        entries.update(versions)
        entries["lumi_id_types"] = "pdg_mc_ids"
        entries["results_pdf"] = self.pdf
        tools.update_grid_metadata(
            self.grid, self.gridtmp, entries, {"results": results_log}
        )
        self.update_with_tmp()

    def postprocess(self):
        """Postprocess grid."""
        # add metadata
        metadata = self.source / "metadata.txt"
        entries = {}
        if metadata.exists():
            for line in metadata.read_text().splitlines():
                k, v = line.split("=")
                entries[k] = v
        tools.update_grid_metadata(self.grid, self.gridtmp, entries)
        self.update_with_tmp()

        # apply postrun, if present
        if os.access((self.source / "postrun.sh"), os.X_OK):
            shutil.copy2(self.source / "postrun.sh", self.dest)
            os.environ["GRID"] = str(self.grid)
            subprocess.run("./postrun.sh", cwd=self.dest, check=True)

        # compress
        compressed_path = tools.compress(self.grid)
        if compressed_path.exists():
            self.grid.unlink()
