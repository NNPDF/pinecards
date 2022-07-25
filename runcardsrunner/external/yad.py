# -*- coding: utf-8 -*-
from functools import reduce

import lhapdf
import pandas as pd
import yadism
import yadism.output
import yaml

from .. import configs, log, tools
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
    return (configs.configs["paths"]["runcards"] / name / "observable.yaml").exists()


class Yadism(interface.External):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        # load runcards
        with open(
            configs.configs["paths"]["runcards"] / self.name / "observable.yaml"
        ) as o:
            self.obs = yaml.safe_load(o)

    @property
    def output(self):
        return self.grid.with_suffix(".tar")

    def run(self):
        print("Running yadism...")

        # run yadism
        run_log = self.dest / "run.log"
        with log.Tee(run_log, stderr=True):
            try:
                out = yadism.run_yadism(self.theory, self.obs)
            except Exception:
                raise log.WhileRedirectedError(file=run_log)

        # dump output
        out.dump_tar(self.output)

    def generate_pineappl(self):
        out = yadism.output.Output.load_tar(self.output)
        out.dump_pineappl_to_file(
            str(self.grid), next(iter(self.obs["observables"].keys()))
        )

    def results(self):
        pdf = lhapdf.mkPDF(self.pdf)
        out = yadism.output.Output.load_tar(self.output)
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
            if len(sv_pdf_out) > 0:
                df.drop(
                    [col for col in df if col in sv_pdf_out[0]], axis=1, inplace=True
                )
            sv_pdf_out.append(df)

        sv_pdf_merged = reduce(
            lambda left, right: pd.merge(
                left,
                right,
                left_index=True,
                right_index=True,
                validate="one_to_one",
            ),
            sv_pdf_out,
        )
        svdf = sv_pdf_merged[
            list(filter(lambda name: isinstance(name, tuple), sv_pdf_merged.columns))
        ]
        pdf_out["sv_max"] = svdf.max(axis=1)
        pdf_out["sv_min"] = svdf.min(axis=1)

        return pdf_out

    def collect_versions(self):
        return {}
