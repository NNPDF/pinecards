#!/bin/bash

# There are two steps to the postprocess:
# Deleting bins that go beyond the available datapoints
# Adding kinematic info for each grid

STEP1="pre_kin_bin"
STEP2="pre_bin_deletion"

mkdir $STEP1
mkdir $STEP2

# Bin mapping
bins[1]="0.0 < y_max < 0.5"
bins[2]="0.5 < y_max < 1.0"
bins[3]="1.0 < y_max < 1.5"
bins[4]="1.5 < y_max < 2.0"
bins[5]="2.0 < y_max < 2.5"

# Bins to be deleted (from the first two grids)
bin_range[1]="22-23"
bin_range[2]="22"
bin_range[3]="none"
bin_range[4]="none"
bin_range[5]="none"


# Convert the grids to pineappl format
for i in {0..4}; do
    j=$((i+1))
    src="grid_00${i}.tab.gz"
    dst="CMS_2JET_13TEV_2D_BIN${j}.pineappl.lz4"
    pineappl import "$src" "$STEP1/$dst" NNPDF40_nnlo_as_01180
    current_bin=${bins[$j]}
    pineappl write --set-key-value kin_bin "${current_bin}" "$STEP1/$dst" "$STEP2/$dst"
    bins_to_delete=${bin_range[$j]}
    if [ ${bins_to_delete} == "none" ]; then
        mv "$STEP2/$dst" $dst
    else
        pineappl write --delete-bins "$bins_to_delete" "$STEP2/$dst" $dst
    fi
done

rm -r $STEP1
rm -r $STEP2
rm *.tab.gz