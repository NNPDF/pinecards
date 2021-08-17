import subprocess
import itertools

import pandas as pd


def compute_data(grid, pdf_name):
    pineappl_results = subprocess.run(
        f"pineappl convolute {grid} {pdf_name} --scales 9 --absolute --integrated".split(),
        capture_output=True,
    )

    output = pineappl_results.stdout.decode().splitlines()[2:-2]
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


def print_table(pineappl_results, external_results):
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

    with pd.option_context(
        "display.max_rows", None, "display.float_format", lambda f: f"{f:.1e}"
    ):
        print(comparison)
