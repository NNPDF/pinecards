#!/bin/bash

mkdir tmp

# Bin mapping
bins[1]="y*=0.25"
bins[2]="y*=0.75"
bins[3]="y*=1.25"
bins[4]="y*=1.75"
bins[5]="y*=2.25"
bins[6]="y*=2.75"

# Convert the grids to pineappl format under the right names
# also, add bin info
for i in {0..5}; do
    j=$((i+1))
    src="grid_00${i}.tab.gz"
    dst="ATLAS_2JET_13TEV_DIF_MJJ-Y_BIN${j}.pineappl.lz4"
    temp_dst="tmp/${dst}"
    pineappl import "$src" "$temp_dst" NNPDF40_nnlo_as_01180
    current_bin=${bins[$j]}
    pineappl write --set-key-value kin_bin "${current_bin}" ${temp_dst} ${dst}
    rm ${temp_dst}
    rm ${src}
done

rmdir tmp