import inspect
import itertools
import subprocess

import lhapdf
import more_itertools
import pandas as pd
import pineappl

from . import tools


def compute_data(grid, pdf_name):
    """Call `convolute` via PineAPPL CLI.

    Parameters
    ----------
    grid : str
        grid path
    pdf_name : str
        PDF name

    Returns
    -------
    list(str)
        (essential) output splitted by line

    """
    pdf = lhapdf.mkPDF(pdf_name)
    loaded_grid = pineappl.grid.Grid.read(str(grid))
    pineappl_results = loaded_grid.convolute_with_one(
        2212, pdf.xfxQ2, pdf.alphasQ2, xi=tools.nine_points
    )

    df = pd.DataFrame(more_itertools.chunked(pineappl_results, len(tools.nine_points)))
    df.rename
    df["sv_max"] = df.max(axis=1)
    df["sv_min"] = df.min(axis=1)
    df.rename(columns={tools.nine_points.index((1.0, 1.0)): "integ"}, inplace=True)

    return df


def parse_pineappl_table(output):
    """Parse PineAPPL CLI output to :class:`~pandas.DataFrame`.

    Parameters
    ----------
    output : list(str) or pd.DataFrame
        output of ``pineappl convolute ...`` or already parsed object (such that
        this function is idempotent)

    Returns
    -------
    pandas.DataFrame
        parsed data

    """
    if isinstance(output, pd.DataFrame):
        return output

    header = output[0].split()
    header = [
        header[0],
        *itertools.chain(*[(e + "_min", e + "_max") for e in header[1:-10]]),
        *header[-10:],
    ]
    output = output[2:]

    df = pd.DataFrame([row.split() for row in output], dtype=float, columns=header)
    df.drop("bin", axis=1, inplace=True)

    svdf = df[list(filter(lambda s: s[0] == "(", df.columns))]
    df["sv_max"] = svdf.max(axis=1)
    df["sv_min"] = svdf.min(axis=1)

    return df


def print_table(pineappl_results, external_results, dest):
    """Print comparison table to screen.

    Parameters
    ----------
    pineappl_results : pandas.DataFrame
        results from the generated grid
    external_results : pandas.DataFrame
        results from the external program
    dest : pathlib.Path
        path to output file

    """
    comparison = pd.DataFrame()

    # bare results
    comparison["PineAPPL"] = pineappl_results["integ"]
    comparison["MC"] = external_results["result"]
    comparison["sigma 1/100"] = (
        external_results["error"] / external_results["result"] * 1e2
    )

    # ratios
    comparison["central sigma"] = (
        pineappl_results["integ"] - external_results["result"]
    ).abs() / external_results["error"]
    comparison["central 1/1000"] = (
        pineappl_results["integ"] / external_results["result"] - 1
    ).abs() * 1e3

    # scale variation ratios
    comparison["min 1/1000"] = (
        pineappl_results["sv_min"] / external_results["sv_min"] - 1.0
    ).abs() * 1e3
    comparison["max 1/1000"] = (
        pineappl_results["sv_max"] / external_results["sv_max"] - 1.0
    ).abs() * 1e3

    comparison.replace(float("inf"), 0.0, inplace=True)

    header = inspect.cleandoc(
        """
        -------------------------------------------------------------------------
        bin   PineAPPL         MC        sigma      central         min      max
                                         1/100   sigma   1/1000   1/1000   1/1000
        -------------------------------------------------------------------------
        """
    )
    exp_float = lambda f: f" {f:.6e}"
    fixed_decimals = lambda w, n: lambda f: f"{{:{w}.{n}f}}".format(f)
    formatters = {
        "PineAPPL": exp_float,
        "MC": exp_float,
        "sigma 1/100": fixed_decimals(7, 3),
        "central sigma": fixed_decimals(7, 3),
        "central 1/1000": fixed_decimals(8, 4),
        "min 1/1000": fixed_decimals(8, 4),
        "max 1/1000": fixed_decimals(8, 4),
    }

    with pd.option_context("display.max_rows", None):
        comp_str = comparison.to_string(index=True, header=False, formatters=formatters)

    comp_str = f"{header}\n{comp_str}"

    with open(dest / "results.log", "w") as fd:
        fd.write(comp_str)
    print(comp_str)
