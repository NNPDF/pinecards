#!/bin/bash
mkdir tmp

# Bin mapping
bins[1]="0.0 < y < 0.5"
bins[2]="0.5 < y < 1.0"
bins[3]="1.0 < y < 1.5"
bins[4]="1.5 < y < 2.0"
bins[5]="2.0 < y < 2.5"

# Convert the grids to pineappl format under the right names
for i in {0..4}; do
    j=$((i+1))
    src="grid_00${i}.tab.gz"
    dst="CMS_2JET_7TEV_M12Y_BIN${j}.pineappl.lz4"
    temp_dst="tmp/${dst}"
    pineappl import "$src" "$temp_dst" NNPDF40_nnlo_as_01180
    current_bin=${bins[$j]}
    pineappl write --set-key-value kin_bin "${current_bin}" ${temp_dst} ${dst}
    rm ${temp_dst}
    rm ${src}
done

rmdir tmp