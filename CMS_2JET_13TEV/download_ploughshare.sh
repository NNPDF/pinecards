#!/bin/bash

# The purpose of this script is to download the dijet grids in question from ploughshare
# and convert them to the pineappl format under the correct names (as specified in commondata)
# Script takes one positional argument: the name of the conda environment to be used
# You need to make sure that your conda environment contains the right ingredients for the grid conversion
# For macOS, you need the following:
# fastNLO: 'brew tap davidchall/hep' 'brew install --without-fastjet fastnlo'
# PineAPPL complied with fastnlo: 'cargo install --features=fastnlo --path pineappl_cli'
# This should enable the command 'pineappl import' to run correctly

ENV_NAME=$1

# conda activate is a shell function and requires conda.sh to be sourced first.
# Find the conda base directory, trying multiple common install locations.
if command -v conda &>/dev/null; then
    CONDA_BASE=$(conda info --base 2>/dev/null)
fi
if [ -z "$CONDA_BASE" ]; then
    for candidate in \
        "$HOME/miniforge3" \
        "$HOME/mambaforge" \
        "$HOME/miniconda3" \
        "$HOME/anaconda3" \
        "/opt/conda" \
        "/opt/miniforge3" \
        "/opt/miniconda3" \
        "/opt/anaconda3" \
        "/usr/local/miniconda3" \
        "/usr/local/anaconda3"
    do
        if [ -f "$candidate/etc/profile.d/conda.sh" ]; then
            CONDA_BASE="$candidate"
            break
        fi
    done
fi
if [ -z "$CONDA_BASE" ]; then
    echo "Error: Could not find conda installation" >&2
    exit 1
fi

# shellcheck source=/dev/null
source "$CONDA_BASE/etc/profile.d/conda.sh"
conda activate "$ENV_NAME"

# Create dir for adjusted grids
GRIDS_2D_PATH="CMS_2JET_13TEV_2D"
GRIDS_3D_PATH="CMS_2JET_13TEV_3D"
mkdir -p "$GRIDS_2D_PATH"
mkdir -p "$GRIDS_3D_PATH"

# Download and extract the grids
FILENAME="applfast-cms-dijets-v2-fc-fnlo-arxiv-2312.16669"
wget "https://ploughshare.web.cern.ch/ploughshare/db/applfast/$FILENAME/${FILENAME}.tgz"
tar -xzvf "${FILENAME}.tgz"
PS_FOLDER="$FILENAME/grids"

# Utility functions
delete_bins() {
    local bin_range="$1"
    local input="$2"
    local output="$3"
    pineappl write --delete-bins "$bin_range" "$input" "$output"
}

add_bin_info() {
    local bin_info="$1"
    local input="$2"
    pineappl write --set-key-value kin_bin "$bin_info" "$input" "${GRIDS_3D_PATH}/${input}"
}

# Convert the grids to pineappl format
for i in {0..19}; do
    if ((i<10)); then
        src="$PS_FOLDER/${FILENAME}-xsec00${i}.tab.gz"
        dst="$PS_FOLDER/${FILENAME}-xsec00${i}.pineappl.lz4"
        pineappl import "$src" "$dst" NNPDF40_nnlo_as_01180
    else
        src="$PS_FOLDER/${FILENAME}-xsec0${i}.tab.gz"
        dst="$PS_FOLDER/${FILENAME}-xsec0${i}.pineappl.lz4"
        pineappl import "$src" "$dst" NNPDF40_nnlo_as_01180
    fi
done

# Rename 2D grids
for i in {0..4}; do
    bin=$((i + 1))
    src="$PS_FOLDER/${FILENAME}-xsec00${i}.pineappl.lz4"
    dst="CMS_2JET_13TEV_2D_BIN${bin}.pineappl.lz4"
    mv "$src" "$dst"
done

# Delete bins from 2D grids
mv "CMS_2JET_13TEV_2D_BIN3.pineappl.lz4" "$GRIDS_2D_PATH/."
mv "CMS_2JET_13TEV_2D_BIN4.pineappl.lz4" "$GRIDS_2D_PATH/."
mv "CMS_2JET_13TEV_2D_BIN5.pineappl.lz4" "$GRIDS_2D_PATH/."
delete_bins 22-23 CMS_2JET_13TEV_2D_BIN1.pineappl.lz4 ${GRIDS_2D_PATH}/CMS_2JET_13TEV_2D_BIN1.pineappl.lz4
delete_bins 22    CMS_2JET_13TEV_2D_BIN2.pineappl.lz4 ${GRIDS_2D_PATH}/CMS_2JET_13TEV_2D_BIN2.pineappl.lz4

# Rename the 3D grids (note that the mapping of the bins is non-trivial)
mv "$PS_FOLDER/${FILENAME}-xsec005.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN1.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec010.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN2.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec014.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN3.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec017.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN4.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec019.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN5.pineappl.lz4"

mv "$PS_FOLDER/${FILENAME}-xsec006.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN2_ys_BIN1.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec011.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN2_ys_BIN2.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec015.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN2_ys_BIN3.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec018.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN2_ys_BIN4.pineappl.lz4"

mv "$PS_FOLDER/${FILENAME}-xsec007.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN3_ys_BIN1.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec012.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN3_ys_BIN2.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec016.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN3_ys_BIN3.pineappl.lz4"

mv "$PS_FOLDER/${FILENAME}-xsec008.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN4_ys_BIN1.pineappl.lz4"
mv "$PS_FOLDER/${FILENAME}-xsec013.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN4_ys_BIN2.pineappl.lz4"

mv "$PS_FOLDER/${FILENAME}-xsec009.pineappl.lz4" "CMS_2JET_13TEV_3D_yb_BIN5_ys_BIN1.pineappl.lz4"

# Add bin information to 3D grids
add_bin_info "yb<0.5 ys<0.5"             CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN1.pineappl.lz4
add_bin_info "yb<0.5 0.5<=ys<1.0"        CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN2.pineappl.lz4
add_bin_info "yb<0.5 1.0<=ys<1.5"        CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN3.pineappl.lz4
add_bin_info "yb<0.5 1.5<=ys<2.0"        CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN4.pineappl.lz4
add_bin_info "yb<0.5 2.0<=ys<2.5"        CMS_2JET_13TEV_3D_yb_BIN1_ys_BIN5.pineappl.lz4

add_bin_info "0.5<=yb<1.0 ys<0.5"        CMS_2JET_13TEV_3D_yb_BIN2_ys_BIN1.pineappl.lz4
add_bin_info "0.5<=yb<1.0 0.5<=ys<1.0"   CMS_2JET_13TEV_3D_yb_BIN2_ys_BIN2.pineappl.lz4
add_bin_info "0.5<=yb<1.0 1.0<=ys<1.5"   CMS_2JET_13TEV_3D_yb_BIN2_ys_BIN3.pineappl.lz4
add_bin_info "0.5<=yb<1.0 1.5<=ys<2.0"   CMS_2JET_13TEV_3D_yb_BIN2_ys_BIN4.pineappl.lz4

add_bin_info "1.0<=yb<1.5 ys<0.5"        CMS_2JET_13TEV_3D_yb_BIN3_ys_BIN1.pineappl.lz4
add_bin_info "1.0<=yb<1.5 0.5<=ys<1.0"   CMS_2JET_13TEV_3D_yb_BIN3_ys_BIN2.pineappl.lz4
add_bin_info "1.0<=yb<1.5 1.0<=ys<1.5"   CMS_2JET_13TEV_3D_yb_BIN3_ys_BIN3.pineappl.lz4

add_bin_info "1.5<=yb<2.0 ys<0.5"        CMS_2JET_13TEV_3D_yb_BIN4_ys_BIN1.pineappl.lz4
add_bin_info "1.5<=yb<2.0 0.5<=ys<1.0"   CMS_2JET_13TEV_3D_yb_BIN4_ys_BIN2.pineappl.lz4

add_bin_info "2.0<=yb<2.5 ys<0.5"        CMS_2JET_13TEV_3D_yb_BIN5_ys_BIN1.pineappl.lz4

rm *.pineappl.lz4
rm -r "$FILENAME"
rm ${FILENAME}.tgz