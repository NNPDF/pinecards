from functools import reduce

import lhapdf
import pandas as pd

from .. import tools


def yadism_results(out, pdf_name):
    pdf = lhapdf.mkPDF(pdf_name)
    pdf_out = out.apply_pdf_alphas_alphaqed_xir_xif(
        pdf,
        lambda muR: lhapdf.mkAlphaS(pdf_name).alphasQ(muR),
        lambda _muR: 0,
        1.0,
        1.0,
    )
    pdf_out = next(iter(pdf_out.tables.values()))

    sv_pdf_out = []
    for xiR, xiF in tools.nine_points:
        sv_point = out.apply_pdf_alphas_alphaqed_xir_xif(
            pdf,
            lambda muR: lhapdf.mkAlphaS(pdf_name).alphasQ(muR),
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
