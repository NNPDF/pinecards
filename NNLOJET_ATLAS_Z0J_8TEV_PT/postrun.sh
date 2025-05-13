#!/usr/bin/bash

#
# This script finalizes the grids and makes them compatible with
# the previous set of NNPDF grids for the PT-Y distribution.
# For the PT-MLL distribution the grids span a different phase space.
# The old NNPDF grid were called ATLASZPT8TEV*
#
# It uses an external $PINECARD environment variable which should point to the PINECARD folder
#
# For this script to be able to finalize the grid correctly (with all metadata)
# it needs access to the Final/ folder in which the information about the grid and .dat combination is stored
#
# Note: the lowPT region might mess up with the statistics and it is convenient
# to run it separated from the region that is actually used in the PDF fit.
# If the lowPT region has not been run separately set the following flag to false
#
# Note 2: this script only works with pineappl >= 1

if [[ -z $PINECARD ]]
then
    # If the external PINECARD variable is not set, set it to the cwd
    PINECARD="."
fi

if [[ -z $FAILFAST ]]
then
    FAILFAST=true
fi

SEPARATED_LOWPT=true
pinecard=$(ls ${PINECARD}/*.yaml)
# This script assumes that there is a .run runcard available in the run folder
nnlojet_runcard=$(ls *.run)
results_pdf="NNPDF40_nnlo_as_01180"
tmp_name="merged.lz4"

merge_and_finalize() {
    # usage: merge_and_finalize $high_pt_grid
    #
    # Takes a high pt grid, autogenerates the naming for the low pt one
    # then merges them together and finally puts in all the metadata that it can find
    high_pt_grid=${1}

    if [[ ! -f ${high_pt_grid} ]]
    then
        echo ${high_pt_grid} not found
        if [[ ${FAILFAST} == true ]]
        then
            echo "Fail and exit"
            exit -1
        fi
        return 0
    fi

    rm -f ${tmp_name}



    prefix=${high_pt_grid%.NNLO*}
    suffix=${high_pt_grid#*.NNLO}
    low_pt_grid=${prefix}-LOWPT.NNLO${suffix}
    pineappl merge ${tmp_name} ${low_pt_grid} ${high_pt_grid}

    results=Final/NNLO${suffix%.pineappl*}.dat
    final_grid=${prefix}${suffix}

	pineappl write ${tmp_name} --set-key-value nnlojet_runcard "$(cat $nnlojet_runcard)" --set-key-value pinecard "$(cat $pinecard)" --set-key-value results "$(cat $results)" --set-key-value results_pdf ${results_pdf} ${final_grid}
    echo Written ${final_grid}
    
    rm -f ${tmp_name}
}

### NNLOJET name  - NNPDF name
## PT-MLL distribution
# In this case the NNPDF grids already start with a pT cut (for mll > 46 GeV, pt > 30  GeV)
# As a result, if we want to consider bins with lower pT, we need to use a `legacy_theory` implementation anyway.
# Therefore, while they are keep here for reference, the old names are not being used

# Bins that have changed
bin_1=("ATLAS_Z0J_8TEV_PT.NNLO.pT_mll_46_66.pineappl.lz4" "ATLASZPT8TEVMDIST-ATLASZPT8TEV-MLLBIN4_ptZ.pineappl.lz4")
bin_2=("ATLAS_Z0J_8TEV_PT.NNLO.pT_mll_116_150.pineappl.lz4" "ATLASZPT8TEVMDIST-ATLASZPT8TEV-MLLBIN6_ptZ.pineappl.lz4")
# 
for bin in bin_1 bin_2
do
    rm -f ${tmp_name}
    declare -n bin_ref=$bin

    high_pt_grid=${bin_ref[0]}
    final_name=${high_pt_grid/.NNLO./.}

    merge_and_finalize ${bin_ref[0]}

    mv ${final_name} ${tmp_name}
    bins=$(pineappl read ${tmp_name} --bins | awk 'END{print $1}')
    pineappl write ${tmp_name} --delete-bins 1-${bins} --scale 0.0 --set-bins 0,2 empty_bin_${tmp_name}
    pineappl merge ${final_name} empty_bin_${tmp_name} ${tmp_name}
    rm -f ${tmp_name} empty_bin_${tmp_name}
done

# Bins that are kept the same (but we keep the new names anyway)
bin_1=("ATLAS_Z0J_8TEV_PT.NNLO.pT_mll_12_20.pineappl.lz4" "ATLASZPT8TEVMDIST-ATLASZPT8TEV-MLLBIN1_ptZ.pineappl.lz4")
bin_2=("ATLAS_Z0J_8TEV_PT.NNLO.pT_mll_20_30.pineappl.lz4" "ATLASZPT8TEVMDIST-ATLASZPT8TEV-MLLBIN2_ptZ.pineappl.lz4")
bin_3=("ATLAS_Z0J_8TEV_PT.NNLO.pT_mll_30_46.pineappl.lz4" "ATLASZPT8TEVMDIST-ATLASZPT8TEV-MLLBIN3_ptZ.pineappl.lz4")
for bin in bin_1 bin_2 bin_3
do
    rm -f ${tmp_name}
    declare -n high_pt_grid=${bin}

    # For these three there's no need to do any merging, just add the metadata
    prefix=${high_pt_grid%.NNLO*}
    suffix=${high_pt_grid#*.NNLO}
    results=Final/NNLO${suffix%.pineappl*}.dat
    final_grid=${prefix}${suffix}

	pineappl write ${high_pt_grid} --set-key-value nnlojet_runcard "$(cat $nnlojet_runcard)" --set-key-value pinecard "$(cat $pinecard)" --set-key-value results "$(cat $results)" --set-key-value results_pdf ${results_pdf} ${final_grid}

    echo Written ${final_grid}
done


## PT-Y distribution
# In this case the old names will  used: ATLASZPT8TEVYDIST-ATLASZPT8TEVYDIST-BIN${i}_ptZ.pineappl.lz4
bin_1="ATLAS_Z0J_8TEV_PT.NNLO.pt_mll_66_116_y_0.0_0.4.pineappl.lz4"
bin_2="ATLAS_Z0J_8TEV_PT.NNLO.pt_mll_66_116_y_0.4_0.8.pineappl.lz4"
bin_3="ATLAS_Z0J_8TEV_PT.NNLO.pt_mll_66_116_y_0.8_1.2.pineappl.lz4"
bin_4="ATLAS_Z0J_8TEV_PT.NNLO.pt_mll_66_116_y_1.2_1.6.pineappl.lz4"
bin_5="ATLAS_Z0J_8TEV_PT.NNLO.pt_mll_66_116_y_1.6_2.0.pineappl.lz4"
bin_6="ATLAS_Z0J_8TEV_PT.NNLO.pt_mll_66_116_y_2.0_2.4.pineappl.lz4"

for i in {1..6}
do
    declare -n high_pt_grid="bin_$i"

    merge_and_finalize ${high_pt_grid}

    intermediate_name=${high_pt_grid/.NNLO./.}
    final_name=ATLASZPT8TEVYDIST-ATLASZPT8TEVYDIST-BIN${i}_ptZ.pineappl.lz4

    # Add a 0 pt bin at the beginning
    rm -f ${tmp_name}
    bins=$(pineappl read ${intermediate_name} --bins | awk 'END{print $1}')
    pineappl write ${intermediate_name} --delete-bins 1-${bins} --scale 0.0 --set-bins 0,2 ${tmp_name}
    pineappl merge ${final_name} ${tmp_name} ${intermediate_name}
    rm -f ${tmp_name}
done
