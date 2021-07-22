import yadism
import yaml
import pathlib
import numpy as np
import argparse

theory = {
    "CKM": "0.97428 0.22530 0.003470 0.22520 0.97345 0.041000 0.00862 0.04030 0.999152",
    "Comments": "LO baseline for small-x res",
    "DAMP": 0,
    "EScaleVar": 1,
    "FNS": "ZM-VFNS",
    "GF": 1.1663787e-05,
    "HQ": "POLE",
    "IC": 0,
    "ID": 0,
    "MP": 0.938,
    "MW": 80.398,
    "MZ": 91.1876,
    "MaxNfAs": 6,
    "MaxNfPdf": 6,
    "ModEv": "EXA",
    "NfFF": 3,
    "PTO": 1,
    "Q0": 1.0,
    "QED": 0,
    "Qedref": 1.777,
    "Qmb": 4.5,
    "Qmc": 2.0,
    "Qmt": 173.07,
    "Qref": 91.2,
    "SIN2TW": 0.23126,
    "SxOrd": "LL",
    "SxRes": 0,
    "TMC": 0,
    "XIF": 1.0,
    "XIR": 1.0,
    "alphaqed": 0.007496251999999999,
    "alphas": 0.11800000000000001,
    "fact_to_ren_scale_ratio": 1.0,
    "global_nx": 0,
    "kDISbThr": 1.0,
    "kDIScThr": 1.0,
    "kDIStThr": 1.0,
    "kbThr": 1.0,
    "kcThr": 1.0,
    "ktThr": 1.0,
    "mb": 4.5,
    "mc": 2.0,
    "mt": 173.07,
}


def run_dataset(runcard, dataset):
    with open(runcard) as o:
        obs = yaml.safe_load(o)
    out = yadism.run_yadism(theory, obs)
    out.dump_pineappl_to_file(
        str(f"{dataset}.pineappl"), next(iter(obs["observables"].keys()))
    )


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("runcard")
    ap.add_argument("dataset")
    args = ap.parse_args()
    run_dataset(args.runcard, args.dataset)
