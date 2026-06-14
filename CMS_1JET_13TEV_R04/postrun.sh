#!/bin/bash

# There are two steps to the postprocess:
# Deleting bins that go beyond the available datapoints
# Adding kinematic info for each grid

STEP1="pre_kin_bin"
STEP2="pre_bin_deletion"

mkdir $STEP1
mkdir $STEP2

# Bin mapping
bins[1]="0.0 <= |y| < 0.5"
bins[2]="0.5 <= |y| < 1.0"
bins[3]="1.0 <= |y| < 1.5"
bins[4]="1.5 <= |y| < 2.0"

# Bins to be deleted (from the first two grids)
bin_range[1]="none"
bin_range[2]="none"
bin_range[3]="19"
bin_range[4]="16-17"

# Convert the grids to pineappl format under the right names.
# We only take the first 4, as they correspond to mu=H_T.
for i in {0..3}; do
    j=$((i+1))
    src="grid_00${i}.appl"
    dst="CMS_1JET_13TEV_DIF_PT-Y-R04_BIN${j}.pineappl.lz4"
    temp_dst="tmp/${dst}"
    pineappl import "$src" "$STEP1/$dst" NNPDF40_nnlo_as_01180
    current_bin=${bins[$j]}
    pineappl write --set-key-value kin_bin "${current_bin}" "$STEP1/$dst" "$STEP2/$dst"
    bins_to_delete=${bin_range[$j]}
    if [ ${bins_to_delete} == "none" ]; then
        mv "$STEP2/$dst" $dst
    else
        pineappl write --delete-bins "$bins_to_delete" "$STEP2/$dst" $dst
    fi
    rm ${src}
done

rm *.appl
rm -r $STEP1
rm -r $STEP2