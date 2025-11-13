#!/usr/bin/bash

#
# This script ensures that the output grid share the name with the
# previous NNPDF grids
#
GRID_NAME=ATLAS_WJ_8TEV_WM-PT.NNLO.pT.pineappl.lz4
FINAL_NAME=ATLAS_WM_JET_8TEV_PT-atlas-atlas-wjets-arxiv-1711.03296-xsec003.pineappl.lz4

if [[ -z $PINECARD ]]
then
    # If the external PINECARD variable is not set, set it to the cwd
    PINECARD="."
fi

pinecard=$(ls ${PINECARD}/*.yaml)
# This script assumes that there is a .run runcard available in the run folder
nnlojet_runcard=$(ls *.run)
results_pdf="NNPDF40_nnlo_as_01180"
results=Final/NNLO.pT.dat

pineappl write ${GRID_NAME} --set-key-value nnlojet_runcard "$(cat $nnlojet_runcard)" --set-key-value pinecard "$(cat $pinecard)" --set-key-value results "$(cat $results)"  --set-key-value results_pdf ${results_pdf} ${FINAL_NAME}
rm ${GRID_NAME}
