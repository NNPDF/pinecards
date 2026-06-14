#!/bin/bash

mkdir tmp

# Bin mapping
bins[1]="0.0 <= |y| < 0.5"
bins[2]="0.5 <= |y| < 1.0"
bins[3]="1.0 <= |y| < 1.5"
bins[4]="1.5 <= |y| < 2.0"
bins[5]="2.0 <= |y| < 2.5"
bins[6]="2.5 <= |y| < 3.0"

# Convert the grids to pineappl format under the right names.
# We only take the first 6, as they correspond to mu=H_T.
for i in {0..5}; do
    j=$((i+1))
    src="grid_00${i}.appl"
    dst="ATLAS_1JET_13TEV_DIF_PT-Y_BIN${j}.pineappl.lz4"
    temp_dst="tmp/${dst}"
    pineappl import "$src" "$temp_dst" NNPDF40_nnlo_as_01180
    current_bin=${bins[$j]}
    pineappl write --set-key-value kin_bin "${current_bin}" ${temp_dst} ${dst}
    rm ${temp_dst}
    rm ${src}
done

# Remove the non-H_T grids
rm *.appl
rm *.root

rmdir tmp