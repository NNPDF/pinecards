#!/bin/bash
mkdir tmp

# Bin mapping
bin1="y_b=0.5, y*=0.5"
bin2="y_b=0.5, y*=1.5"
bin3="y_b=0.5, y*=2.5"
bin4="y_b=1.5, y*=0.5"
bin5="y_b=1.5, y*=1.5"
bin6="y_b=2.5, y*=0.5"

# Convert the grids to pineappl format under the right names
for i in {0..5}; do
    j=$((i+1))
    src="grid_${i}.tab.gz"
    temp_dst="tmp/CMS_2JET_8TEV_3D_BIN${j}.pineappl.lz4"
    dst="CMS_2JET_8TEV_3D_BIN${j}.pineappl.lz4"
    pineappl import "$src" "$temp_dst" NNPDF40_nnlo_as_01180
    eval "current_bin=\$bin${j}"
    pineappl write --set-key-value kin_bin "${current_bin}" ${temp_dst} ${dst}
    rm ${temp_dst}
done

rmdir tmp
rm "tmp.pineappl.lz4"