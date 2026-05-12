#!/bin/bash

# There are two steps to the postprocess:
# Deleting bins that go beyond the available datapoints
# Adding kinematic info for each grid

STEP1="pre_bin_deletion"
STEP2="pre_kin_bin"

mkdir $STEP1
mkdir $STEP2

# Bin mapping
bins[1]="0.0 <= |y| < 0.5"
bins[2]="0.5 <= |y| < 1.0"
bins[3]="1.0 <= |y| < 1.5"
bins[4]="1.5 <= |y| < 2.0"
bins[5]="2.0 <= |y| < 2.5"

# Bins to be deleted
bin_range[1]="28-32"
bin_range[2]="28-29"
bin_range[3]="25-26"
bin_range[4]="21-23"
bin_range[5]="16-18"

# Convert the grids to pineappl format under the right names.
# We only take the first 5, as they correspond to mu=H_T.
for i in {0..4}; do
    j=$((i+1))
    src="grid_00${i}.appl"
    dst="CMS_1JET_7TEV_R07_BIN${j}.pineappl.lz4"
    pineappl import "$src" "$STEP1/$dst" NNPDF40_nnlo_as_01180
    bins_del=${bin_range[$j]}
    pineappl write --delete-bins "${bins_del}" "$STEP1/$dst" "$STEP2/$dst"
    current_bin=${bins[$j]}
    pineappl write --set-key-value kin_bin "${current_bin}" "$STEP2/$dst" ${dst}
    rm ${src}
done

# Remove temporary files
rm -r $STEP1
rm -r $STEP2

# Remove the non-H_T grids
rm *.appl
rm *.root