from functools import reduce

import pandas as pd
import yaml
import yadism
import lhapdf

from .. import paths, table, tools
from . import interface


class Yadism(interface.External):
    def __init__(self, *args):
        super().__init__(*args)
        self.out = None

    def run(self):
        print("Running yadism...")

        # load runcards
        with open(paths.pkg / "theory.yaml") as t:
            theory = yaml.safe_load(t)
        with open(paths.runcards / self.name / "observable.yaml") as o:
            obs = yaml.safe_load(o)

        # run yadism
        self.out = yadism.run_yadism(theory, obs)

        # dump pineappl
        self.out.dump_pineappl_to_file(
            str(self.grid), next(iter(obs["observables"].keys()))
        )

    def results(self):
        pdf = lhapdf.mkPDF(self.pdf)
        pdf_out = self.out.apply_pdf_alphas_alphaqed_xir_xif(
            pdf,
            lambda muR: lhapdf.mkAlphaS(self.pdf).alphasQ(muR),
            lambda _muR: 0,
            1.0,
            1.0,
        )
        pdf_out = next(iter(pdf_out.tables.values()))

        sv_pdf_out = []
        for xiR, xiF in tools.nine_points:
            sv_point = self.out.apply_pdf_alphas_alphaqed_xir_xif(
                pdf,
                lambda muR: lhapdf.mkAlphaS(self.pdf).alphasQ(muR),
                lambda _muR: 0.0,
                xiR,
                xiF,
            )
            df = (
                next(iter(sv_point.tables.values()))
                .rename({"result": (xiR, xiF)}, axis=1)
                .drop("error", axis=1)
            )
            sv_pdf_out.append(df)

        sv_pdf_merged = reduce(
            lambda left, right: pd.merge(left, right, on=["x", "Q2"], how="outer"),
            sv_pdf_out,
        )
        svdf = sv_pdf_merged[
            list(filter(lambda name: isinstance(name, tuple), sv_pdf_merged.columns))
        ]
        pdf_out["sv_max"] = svdf.max(axis=1)
        pdf_out["sv_min"] = svdf.min(axis=1)

        return pdf_out

    def generate_pineappl(self):
        return table.compute_data(self.grid, self.pdf)

    def collect_versions(self):
        return {}

    def postprocess(self):
        # compress
        compressed_path = tools.compress(self.grid)
        if compressed_path.exists():
            self.grid.unlink()
