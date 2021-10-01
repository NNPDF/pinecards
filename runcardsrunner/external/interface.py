import abc
import shutil
import subprocess

import pygit2
from pkgconfig.pkgconfig import exists

from .. import paths, tools


class External(abc.ABC):
    def __init__(self, name, pdf, timestamp=None):
        self.name = name
        self.pdf = pdf
        self.timestamp = timestamp

        if timestamp is None:
            self.dest = tools.create_folder(self.name)
        else:
            self.dest = paths.root / (self.name + "-" + self.timestamp)
            if not self.grid.exists():
                tools.decompress(self.grid.with_suffix(".pineappl.lz4"))

    @property
    def source(self):
        return paths.runcards / self.name

    @property
    def grid(self):
        return self.dest / f"{self.name}.pineappl"

    @property
    def gridtmp(self):
        return self.dest / f"{self.name}.pineappl.tmp"

    def update_with_tmp(self):
        shutil.move(self.gridtmp, self.grid)

    @staticmethod
    def install():
        pass

    @abc.abstractmethod
    def run(self):
        pass

    @abc.abstractproperty
    def results(self):
        pass

    @abc.abstractproperty
    def generate_pineappl(self):
        pass

    @abc.abstractmethod
    def collect_versions(self):
        pass

    def annotate_versions(self):
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
        pass
