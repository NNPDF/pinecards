import abc
import os
import shutil
import subprocess

import pygit2

from .. import paths, tools


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
            self.dest = paths.root / (self.name + "-" + self.timestamp)
            if not self.grid.exists():
                tools.decompress(self.grid.with_suffix(".pineappl.lz4"))

    @property
    def source(self):
        """Runcard base directory."""
        return paths.runcards / self.name

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
        """Generate PineAPPL output."""

    @abc.abstractmethod
    def results(self):
        """Results as computed by the program."""

    @abc.abstractmethod
    def collect_versions(self):
        """Collect necessary version informations."""

    def annotate_versions(self):
        """Add version informations as meta data."""
        results_log = self.dest / "results.log"
        # TODO: add pineappl version
        #  pineappl = paths.pineappl_exe()

        versions = self.collect_versions()
        versions["runcard_gitversion"] = pygit2.Repository(paths.root).describe(
            always_use_long_format=True,
            describe_strategy=pygit2.GIT_DESCRIBE_TAGS,
            dirty_suffix="-dirty",
            show_commit_oid_as_fallback=True,
        )

        entries = {}
        entries.update(versions)
        entries["lumi_id_types"] = "pdg_mc_ids"
        tools.set_grid_metadata(
            self.grid, self.gridtmp, entries, {"results": results_log}
        )
        self.update_with_tmp()

    def postprocess(self):
        """Postprocess grid."""
        if os.access((self.source / "postrun.sh"), os.X_OK):
            shutil.copy2(self.source / "postrun.sh", self.dest)
            os.environ["GRID"] = str(self.grid)
            subprocess.run("./postrun.sh", cwd=self.dest, check=True)

        # compress
        compressed_path = tools.compress(self.grid)
        if compressed_path.exists():
            self.grid.unlink()
