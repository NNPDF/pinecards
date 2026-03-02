# here we produce two pineappl grids, such that we find A4 upon division of the first by the second
import pathlib
import pineappl
import subprocess

grid_dir = pathlib.Path("./2408_07622_A4_ZJ_NLO/result/final")
grids_to_concat_proj = [grid_dir / f"nlo.mll_bin{i}_proj.pineappl.lz4" for i in range(1, 10)]
grids_to_concat_unproj = [grid_dir / f"nlo.mll_bin{i}.pineappl.lz4" for i in range(1, 10)]

output_dir = pathlib.Path("./results")
output_path_proj = output_dir / "CMS_Z0_13TEV_PROJ_A4.pineappl.lz4"
output_path_unproj = output_dir / "CMS_Z0_13TEV_TOT.pineappl.lz4"
output_path_proj_temp = output_dir / "CMS_Z0_13TEV_PROJ_A4_temp.pineappl.lz4"
output_path_unproj_temp = output_dir / "CMS_Z0_13TEV_TOT_temp.pineappl.lz4"

remapping_strings = {
    1: "54, 66, 76, 82, 86, 89.5, 92.7, 96, 100, 106, 116, 150; 0.0, 0.4",
    2: "54, 66, 76, 82, 86, 89.5, 92.7, 96, 100, 106, 116, 150; 0.4, 0.8",
    3: "54, 66, 76, 82, 86, 89.5, 92.7, 96, 100, 106, 116, 150; 0.8, 1.2",
    4: "54, 66, 76, 86, 96, 106, 116, 150; 1.2, 1.6",
    5: "54, 66, 76, 86, 96, 106, 116, 150; 1.6, 2.0",
    6: "54, 66, 76, 86, 96, 106, 116, 150; 2.0, 2.4",
    7: "66, 86, 96, 116; 2.4, 2.7",
    8: "66, 86, 96, 116; 2.7, 3.0",
    9: "66, 86, 96, 116; 3.0, 3.4",
}

# postrun for the projected grids
temp_output_grids = []
for i, grid in enumerate(grids_to_concat_proj):
    output_grid = output_dir / f"{grid.name.replace('.pineappl.lz4', '_temp.pineappl.lz4')}"
    subprocess.run(
                ["pineappl", "write", "--set-bins", remapping_strings[i + 1], grid, output_grid]
            )
    temp_output_grids.append(output_grid)

subprocess.run(["pineappl", "merge", output_path_proj_temp] + temp_output_grids)
subprocess.run(["pineappl", "write", "--set-key-value", "x1_label", "mll", "--set-key-value", "x2_label", "yll", output_path_proj_temp, output_path_proj])



# postrun for the unprojected grids
temp_output_grids = []
for i, grid in enumerate(grids_to_concat_unproj):
    output_grid = output_dir / f"{grid.name.replace('.pineappl.lz4', '_temp.pineappl.lz4')}"
    subprocess.run(
                ["pineappl", "write", "--set-bins", remapping_strings[i + 1], grid, output_grid]
            )
    temp_output_grids.append(output_grid)

subprocess.run(["pineappl", "merge", output_path_unproj_temp] + temp_output_grids)
subprocess.run(["pineappl", "write", "--set-key-value", "x1_label", "mll", "--set-key-value", "x2_label", "yll", output_path_unproj_temp, output_path_unproj])

