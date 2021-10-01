import abc
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
        pdf : str
            PDF name
    """

    def __init__(self, name, pdf):
        self.name = name
        self.pdf = pdf
        self.dest = tools.create_folder(self.name)

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

    @abc.abstractproperty
    def results(self):
        """Results as computed by the program."""

    @abc.abstractmethod
    def generate_pineappl(self):
        """Generate PineAPPL output."""

    @abc.abstractmethod
    def collect_versions(self):
        """Collect necessary version informations."""

    def annotate_versions(self):
        """Add version informations as meta data."""
        results_log = self.dest / "results.log"
        pineappl = paths.pineappl_exe()

        versions = {}
        versions["runcard_gitversion"] = pygit2.Repository(paths.root).describe(
            always_use_long_format=True,
            describe_strategy=pygit2.GIT_DESCRIBE_TAGS,
            dirty_suffix="-dirty",
            show_commit_oid_as_fallback=True,
        )

        entries = []
        for name, version in versions.items():
            entries += ["--entry", name, version]
        entries += ["--entry", "lumi_id_types", "pdg_mc_ids"]
        subprocess.run(
            f"{pineappl} set {self.grid} {self.gridtmp}".split()
            + f"--entry_from_file results {results_log}".split()
            + entries
        )
        self.update_with_tmp()

    @abc.abstractmethod
    def postprocess(self):
        """Postprocess grid."""
