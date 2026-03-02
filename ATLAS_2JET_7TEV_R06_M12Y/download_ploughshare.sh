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

# Create dir for adjusted grids (and a tmp directory)
GRIDS_PATH="ATLAS_2JET_7TEV_R06"
mkdir -p "$GRIDS_PATH"
mkdir -p "$GRIDS_PATH/tmp"

# Download and extract the grids
FILENAME="applfast-atlas-dijets-v2-fc-fnlo-arxiv-1312.3524"
wget "https://ploughshare.web.cern.ch/ploughshare/db/applfast/$FILENAME/${FILENAME}.tgz"
tar -xzvf "${FILENAME}.tgz"
PS_FOLDER="$FILENAME/grids"

# Bin mapping
bin1="y*=0.25"
bin2="y*=0.75"
bin3="y*=1.25"
bin4="y*=1.75"
bin5="y*=2.25"
bin6="y*=2.75"

# Convert the grids to pineappl format under the right names
# also, add bin info
for i in {0..5}; do
    j=$((i+1))
    src="$PS_FOLDER/${FILENAME}-xsec00${i}.tab.gz"
    temp_dst="$GRIDS_PATH/tmp/ATLAS_2JET_7TEV_R06_M12Y_BIN${j}.pineappl.lz4"
    dst="$GRIDS_PATH/ATLAS_2JET_7TEV_R06_M12Y_BIN${j}.pineappl.lz4"
    pineappl import "$src" "$temp_dst" NNPDF40_nnlo_as_01180
    eval "current_bin=\$bin${j}"
    pineappl write --set-key-value kin_bin ${current_bin} ${temp_dst} ${dst}
    rm ${temp_dst}
done

rmdir "$GRIDS_PATH/tmp"
rm -r "$FILENAME"
rm ${FILENAME}.tgz