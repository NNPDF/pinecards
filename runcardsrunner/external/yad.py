from functools import reduce

import lhapdf
import pandas as pd
import yadism
import yadism.output
import yaml

from .. import install, paths, table, tools
from . import interface


def is_dis(name):
    """
    Is this a DIS dataset, i.e. is yadism needed to run?

    The decision is based on the existance of the `observable.yaml` file.

    Parameters
    ----------
        name : str
            dataset name
    """
    return (paths.runcards / name / "observable.yaml").exists()


class Yadism(interface.External):
    @staticmethod
    def install():
        install.lhapdf_conf()

    def run(self):
        print("Running yadism...")

        # load runcards
        with open(paths.runcards / self.name / "observable.yaml") as o:
            obs = yaml.safe_load(o)

        # run yadism
        out = yadism.run_yadism(self.theory, obs)

        # dump pineappl
        out.dump_pineappl_to_file(str(self.grid), next(iter(obs["observables"].keys())))
        out.dump_yaml_to_file(self.grid.with_suffix(".yaml"))

    def results(self):
        pdf = lhapdf.mkPDF(self.pdf)
        out = yadism.output.Output.load_yaml_from_file(self.grid.with_suffix(".yaml"))
        pdf_out = out.apply_pdf_alphas_alphaqed_xir_xif(
            pdf,
            lambda muR: lhapdf.mkAlphaS(self.pdf).alphasQ(muR),
            lambda _muR: 0,
            1.0,
            1.0,
        )
        pdf_out = next(iter(pdf_out.tables.values()))

        sv_pdf_out = []
        for xiR, xiF in tools.nine_points:
            sv_point = out.apply_pdf_alphas_alphaqed_xir_xif(
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
