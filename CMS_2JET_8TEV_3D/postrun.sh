#!/bin/bash
mkdir tmp

# Bin mapping
bins[1]="y_b=0.5, y*=0.5"
bins[2]="y_b=0.5, y*=1.5"
bins[3]="y_b=0.5, y*=2.5"
bins[4]="y_b=1.5, y*=0.5"
bins[5]="y_b=1.5, y*=1.5"
bins[6]="y_b=2.5, y*=0.5"

# Convert the grids to pineappl format under the right names
for i in {0..5}; do
    j=$((i+1))
    src="grid_${i}.tab.gz"
    temp_dst="tmp/CMS_2JET_8TEV_3D_BIN${j}.pineappl.lz4"
    dst="CMS_2JET_8TEV_3D_BIN${j}.pineappl.lz4"
    pineappl import "$src" "$temp_dst" NNPDF40_nnlo_as_01180
    current_bin=${bins[$j]}
    pineappl write --set-key-value kin_bin "${current_bin}" ${temp_dst} ${dst}
    rm ${temp_dst}
    rm ${src}
done

rmdir tmp