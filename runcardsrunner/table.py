import inspect
import itertools
import subprocess

import pandas as pd


def compute_data(grid, pdf_name):
    pineappl_results = subprocess.run(
        f"pineappl convolute {grid} {pdf_name} --scales 9 --absolute --integrated".split(),
        capture_output=True,
    )
    return pineappl_results.stdout.decode().splitlines()[2:-2]


def parse_pineappl_table(output):
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
        ----------------------------------------------------------------------
           PineAPPL         MC        sigma      central         min      max
                                      1/100   sigma   1/1000   1/1000   1/1000
        ----------------------------------------------------------------------
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
        comp_str = comparison.to_string(
            index=False, header=False, formatters=formatters
        )

    comp_str = f"{header}\n{comp_str}"

    with open(dest / "results.log", "w") as fd:
        fd.write(comp_str)
    print(comp_str)
