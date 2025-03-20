#!/usr/bin/bash

#
# This script finalizes the grids and makes them compatible with
# the previous set of NNPDF grids 
# (which were called CMSZDIFF12-CMSZDIFF12-BINX)
#
# For this script to be able to finalize the grid correctly (with all metadata)
# it needs access to the folder (remote or local) in which the grids were combined.
# Please modify the remote_path directory below accordingly.
#
#
# Note: the lowPT region might mess up with the statistics and it is convenient
# to run it separated from the region that is actually used in the PDF fit.
# If the lowPT region has not been run separately set the following flag to false
#
# Note 2: this script only works with pineappl >= 1


remote_path=""
get_results() {
    bin=${1}
    data_file=combined/Final/NNLO.pT_bin_${i}.dat

    if [[ -z ${remote_path} ]]
    then
        cp ${data_file} results.dat
    else
        scp ${remote_path}/${data_file} results.dat
    fi
}

# In principle, if there was no local modification we can autogenerate the LO runcard
# this is enough for NNLOJET dokan to run the calculation again
nnlojet_runcard=LO/runcard_production.run

SEPARATED_LOWPT=true
low_pt_grid=CMS_Z0J_8TEV_PT-Y-LOWPT.NNLO.abs_yz.pineappl.lz4

pinecard=$(ls *.yaml)
results_pdf=NNPDF40_nnlo_as_01180

nnlojet_runcard=runcard.run
results=results.dat

for i in {0..4}
do
	input_grid=CMS_Z0J_8TEV_PT-Y.NNLO.pT_bin_${i}.pineappl.lz4

    # Get the results as given by NNLOJET
    get_results ${1}

    if [ "$SEPARATED_LOWPT" = true ]
    then
        # Prepare the combination of bins to remove
        remove_bins=$(seq 0 4 | tr '\n' ',' | sed "s/$i,//" | sed 's/,$//')

        # Use as output name the name from the old NNPDF grids
        # output_name=CMS_Z0J_8TEV_PT-Y-LOWPT.NNLO.pT_bin_${i}.pineappl.lz4
        output_name=CMSZDIFF12-CMSZDIFF12-BIN$(( i+1 ))_ptZ.pineappl.lz4 

        # Make the bins compatible
        rm -f tmp.lz4
        pineappl write $low_pt_grid --delete-bins "${remove_bins}" --set-bins "2,20" --set-key-value x1_label "pT_bin_${i}" tmp.lz4

        # Merge!
        rm -f merged_tmp.lz4
        pineappl merge merged_tmp.lz4 tmp.lz4 $input_grid
        rm -f tmp.lz4

        # And now the input grid is merged_tmp.lz4
        input_grid=merged_tmp.lz4
    fi

	# And, finally, set up the metadata
	pineappl write ${input_grid} --set-key-value nnlojet_runcard "$(cat $nnlojet_runcard)" --set-key-value pinecard "$(cat $pinecard)" --set-key-value results "$(cat $results)" --set-key-value results_pdf ${results_pdf} ${output_name}
done
