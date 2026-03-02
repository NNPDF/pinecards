#!/usr/bin/env python3
from pathlib import Path
from datetime import date
from typing import Dict, Tuple, List
import numpy as np

EWK_BASE = Path("ttbar_ewk_corrns")
QCD_BASE = Path("ttbar_qcd_corrns")

DATASETS = [
    "ATLAS_TTBAR_13P6TEV_TOT_X-SEC",
    "ATLAS_TTBAR_13TEV_TOT_X-SEC",
    "ATLAS_TTBAR_8TEV_TOT_X-SEC",
    "ATLAS_TTBAR_7TEV_TOT_X-SEC",
    "ATLAS_TTBAR_5TEV_TOT_X-SEC",
    "ATLAS_TTBAR_13TEV_HADR_DIF_MTTBAR",
    "ATLAS_TTBAR_13TEV_HADR_DIF_MTTBAR-YTTBAR",
    "ATLAS_TTBAR_13TEV_HADR_DIF_YTTBAR",
    "ATLAS_TTBAR_13TEV_LJ_DIF_MTTBAR",
    "ATLAS_TTBAR_13TEV_LJ_DIF_MTTBAR-PTT",
    "ATLAS_TTBAR_13TEV_LJ_DIF_PTT",
    "ATLAS_TTBAR_13TEV_LJ_DIF_PTT-YT",
    "ATLAS_TTBAR_13TEV_LJ_DIF_YT",
    "ATLAS_TTBAR_13TEV_LJ_DIF_YTTBAR",
    "ATLAS_TTBAR_8TEV_2L_DIF_MTTBAR",
    "ATLAS_TTBAR_8TEV_2L_DIF_YTTBAR",
    "ATLAS_TTBAR_8TEV_LJ_DIF_MTTBAR",
    "ATLAS_TTBAR_8TEV_LJ_DIF_YTTBAR",
    "ATLAS_TTBAR_8TEV_LJ_DIF_YT",
    "ATLAS_TTBAR_8TEV_LJ_DIF_PTT",
    "CMS_TTBAR_13TEV_2L_138FB-1_DIF_MTTBAR",
    "CMS_TTBAR_13TEV_2L_138FB-1_DIF_MTTBAR-YTTBAR",
    "CMS_TTBAR_13TEV_2L_138FB-1_DIF_PTT",
    "CMS_TTBAR_13TEV_2L_138FB-1_DIF_YT",
    "CMS_TTBAR_13TEV_2L_DIF_MTTBAR",
    "CMS_TTBAR_13TEV_2L_DIF_PTT",
    "CMS_TTBAR_13TEV_2L_DIF_YT",
    "CMS_TTBAR_13TEV_2L_DIF_YTTBAR",
    "CMS_TTBAR_13TEV_LJ_DIF_MTTBAR",
    "CMS_TTBAR_13TEV_LJ_DIF_MTTBAR-YTTBAR",
    "CMS_TTBAR_13TEV_LJ_DIF_PTT",
    "CMS_TTBAR_13TEV_LJ_DIF_YT",
    "CMS_TTBAR_13TEV_LJ_DIF_YTTBAR",
    "CMS_TTBAR_8TEV_2L_DIF_MTTBAR-YT",
    "CMS_TTBAR_8TEV_2L_DIF_MTTBAR-YTTBAR",
    "CMS_TTBAR_8TEV_2L_DIF_PTT-YT",
]

HistKey = Tuple[int, str]  # (hist_id, title)

def find_hwu_file(dataset_root: Path) -> Path:
    run_dir = dataset_root / dataset_root.name / "Events" / "run_01"
    if not run_dir.is_dir():
        raise FileNotFoundError(f"Missing run_01 directory: {run_dir}")

    for name in ("MADatNLO.HwU", "MADatNLO.HWU", "MADatNLO.Hwu", "MADatNLO.hwu"):
        p = run_dir / name
        if p.is_file():
            return p
    raise FileNotFoundError(f"No MADatNLO.HwU-like file found in: {run_dir}")

def parse_hwu(path: Path) -> Dict[HistKey, np.ndarray]:
    """
    Parse HwU histograms into arrays.
    Keeps all numeric columns inside <histogram> blocks.
    """
    hists: Dict[HistKey, List[List[float]]] = {}
    current_key: HistKey | None = None
    in_hist = False

    with path.open("r", encoding="utf-8", errors="replace") as f:
        for raw in f:
            line = raw.strip()
            if not line:
                continue

            if line.startswith("<histogram>"):
                parts = line.split(None, 2)
                hid = int(parts[1])
                title = ""
                if len(parts) >= 3:
                    rest = parts[2].strip()
                    if rest.startswith('"') and rest.endswith('"'):
                        title = rest[1:-1]
                    else:
                        title = rest.strip('"')
                current_key = (hid, title)
                hists.setdefault(current_key, [])
                in_hist = True
                continue

            if line in ("</histogram>", "<\\histogram>", "<\\\\histogram>"):
                current_key = None
                in_hist = False
                continue

            if not in_hist:
                continue

            # Skip comments inside hist blocks
            if line.startswith("#") or line.startswith("##"):
                continue

            parts = line.split()
            try:
                row = [float(x) for x in parts]
            except ValueError:
                continue

            if current_key is not None:
                hists[current_key].append(row)

    out: Dict[HistKey, np.ndarray] = {}
    for k, rows in hists.items():
        out[k] = np.array(rows, dtype=float) if rows else np.zeros((0, 0), dtype=float)
    return out

def compute_binwise_k_and_err(ewk: np.ndarray, qcd: np.ndarray):
    """
    Expect columns: xmin xmax y dy (at least)
    Returns K, dK arrays (bin-by-bin).
    """
    if ewk.shape != qcd.shape:
        raise ValueError(f"Shape mismatch EWK {ewk.shape} vs QCD {qcd.shape}")
    if ewk.shape[1] < 4:
        raise ValueError(f"Need >=4 columns (xmin xmax y dy), got {ewk.shape[1]}")

    # check binning
    if not (np.allclose(ewk[:, 0], qcd[:, 0]) and np.allclose(ewk[:, 1], qcd[:, 1])):
        raise ValueError("Bin edges differ between EWK and QCD")

    yE, dyE = ewk[:, 2], ewk[:, 3]
    yQ, dyQ = qcd[:, 2], qcd[:, 3]

    with np.errstate(divide="ignore", invalid="ignore"):
        K = np.where(yQ != 0.0, yE / yQ, np.nan)
        relE = np.where(yE != 0.0, dyE / yE, 0.0)
        relQ = np.where(yQ != 0.0, dyQ / yQ, 0.0)
        dK = np.abs(K) * np.sqrt(relE**2 + relQ**2)

    return K, dK

def compute_integrated_k_and_err(ewk: np.ndarray, qcd: np.ndarray):
    """
    Integrated K = sum(yE)/sum(yQ).
    MC error on sum taken as sqrt(sum(dy^2)) (uncorrelated bins),
    then propagated to K.
    """
    if ewk.shape != qcd.shape:
        raise ValueError(f"Shape mismatch EWK {ewk.shape} vs QCD {qcd.shape}")
    if ewk.shape[1] < 4:
        raise ValueError(f"Need >=4 columns (xmin xmax y dy), got {ewk.shape[1]}")

    yE, dyE = ewk[:, 2], ewk[:, 3]
    yQ, dyQ = qcd[:, 2], qcd[:, 3]

    sumE = float(np.sum(yE))
    sumQ = float(np.sum(yQ))

    # quadrature on sum
    sigE = float(np.sqrt(np.sum(dyE**2)))
    sigQ = float(np.sqrt(np.sum(dyQ**2)))

    with np.errstate(divide="ignore", invalid="ignore"):
        Kint = sumE / sumQ if sumQ != 0.0 else np.nan
        relE = (sigE / sumE) if sumE != 0.0 else 0.0
        relQ = (sigQ / sumQ) if sumQ != 0.0 else 0.0
        dKint = abs(Kint) * np.sqrt(relE**2 + relQ**2)

    return Kint, dKint

def write_cf_like(path: Path, setname: str, codes_used: str, 
                  values: List[Tuple[float, float]]):
    """
    Writes a CF-style file: header between lines of '*' and then lines 'value error'.
    """
    path.parent.mkdir(parents=True, exist_ok=True)

    header = [
        "*" * 80,
        f"SetName: {setname}",
        f"Author: Emanuele R. Nocera (emanueleroberto.nocera@unito.it",
        f"Date: {date.today().isoformat()}",
        f"CodesUsed: {codes_used}",
        "TheoryInput: 41000000",
        "PDFset: NNPDF40_nnlo_as_01180_qed",
        "Warnings: ",
        "*" * 80,
    ]

    with path.open("w", encoding="utf-8") as f:
        f.write("\n".join(header) + "\n")
        for v, ev in values:
            if np.isnan(v) or np.isnan(ev):
                f.write("nan nan\n")
            else:
                # Match the spirit of your example: value (float) + error (5 decimals)
                f.write(f"{v:.5f} {ev:.5f}\n")

def main():
    for ds in DATASETS:
        ewk_root = EWK_BASE / ds
        qcd_root = QCD_BASE / ds

        if not ewk_root.is_dir() or not qcd_root.is_dir():
            print(f"[SKIP] Missing dataset folder(s) for {ds}")
            continue

        try:
            ewk_file = find_hwu_file(ewk_root)
            qcd_file = find_hwu_file(qcd_root)
        except FileNotFoundError as e:
            print(f"[SKIP] {ds}: {e}")
            continue

        ewk_h = parse_hwu(ewk_file)
        qcd_h = parse_hwu(qcd_file)

        common = sorted(set(ewk_h.keys()) & set(qcd_h.keys()))
        if not common:
            print(f"[SKIP] {ds}: no common histograms found")
            continue

        if len(common) > 1:
            print(f"[WARN] {ds}: multiple histograms found ({len(common)}). Using the first one: {common[0]}")

        key = common[0]
        hid, title = key
        ewk_arr = ewk_h[key]
        qcd_arr = qcd_h[key]

        if ewk_arr.size == 0:
            print(f"[SKIP] {ds}: empty histogram")
            continue

        # Bin-by-bin K
        try:
            K, dK = compute_binwise_k_and_err(ewk_arr, qcd_arr)
        except Exception as exc:
            print(f"[SKIP] {ds}: cannot compute binwise K: {exc}")
            continue

        # Integrated K for normalized dataset
        try:
            Kint, dKint = compute_integrated_k_and_err(ewk_arr, qcd_arr)
        except Exception as exc:
            print(f"[SKIP] {ds}: cannot compute integrated K: {exc}")
            continue

        # 1) Absolute dataset file (one file per dataset)
        abs_out = Path(f"K-factors/CF_EWK_{ds}.dat")
        abs_values = list(zip(K.tolist(), dK.tolist()))
        write_cf_like(
            path=abs_out,
            setname=ds,
            codes_used="aMC@NLO (HwU ratio EWK/QCD)",
            values=abs_values,
        )

        # 2) Normalised dataset file (same bins + one extra '-INTEGRATED' entry)
        norm_out = Path(f"K-factors/CF_EWK_{ds}-INTEGRATED.dat")
        norm_values = [(float(Kint), float(dKint))] * len(K)
        write_cf_like(
            path=norm_out,
            setname=ds,  # dataset name is the same
            codes_used="aMC@NLO (HwU ratio EWK/QCD + integrated point)",
            values=norm_values,
        )

        print(f"[OK] {ds}: wrote {abs_out.name} and {norm_out.name}")

    print("Done.")

if __name__ == "__main__":
    main()
    
