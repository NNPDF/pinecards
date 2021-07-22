import subprocess
import io
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

    return df


def print_table(pineappl_results, external_results):
    labels = [
        "PineAPPL",
        "MC",
        "sigma 1/100",
        "central sigma",
        "min 1/1000",
        "max 1/1000",
    ]
    comparison = pd.DataFrame()
    comparison["PineAPPL"] = pineappl_results["integ"]
    comparison["MC"] = external_results["result"]
    comparison["sigma 1/100"] = (
        external_results["error"] / external_results["result"] * 100
    ).replace(float("inf"), 0.0)
    comparison["central sigma"] = (
        (pineappl_results["integ"] - external_results["result"]).abs()
        / external_results["error"]
    ).replace(float("inf"), 0.0)
    comparison["central 1/1000"] = (
        (pineappl_results["integ"] / external_results["result"] - 1).abs() * 1000
    ).replace(float("inf"), 0.0)
    comparison["min 1/1000"] = 0.0
    comparison["max 1/1000"] = 0.0
    print(comparison)
