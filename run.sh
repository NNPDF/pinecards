#!/bin/bash

# exit script at the first sign of an error
set -o errexit

# the following exits if undeclared variables are used
set -o nounset

# exit if some program in a pipeline fails
set -o pipefail

# check arguments of the script
if (( $# != 1 )); then
    echo "Usage: ./run.sh [nameset]" >&2
    echo "  The following namesets are available:" >&2

    for i in $(find nnpdf31_proc/analyses -name *.f); do
        name=${i##*/}
        nameset=${name%%.f}
        echo "  - ${nameset}" >&2
    done

    exit 2
fi

# name of the experiment
experiment="$1"

mg5amc=$(which mg5_aMC 2> /dev/null || true)

if [[ ! -x ${mg5amc} ]]; then
    echo "The binary \`mg5_aMC\` wasn't found. Please adjust your PATH variable" >&2
    exit 1
fi

applcheck=$(which applcheck 2> /dev/null || true)

if [[ ! -x ${applcheck} ]]; then
    echo "The binary \`applcheck\` wasn't found. Please adjust your PATH variable" >&2
    exit 1
fi

merge_bins=$(which merge_bins 2> /dev/null || true)

if [[ ! -x ${merge_bins} ]]; then
    echo "The binary \`merge_bins\` wasn't found. Please adjust your PATH variable" >&2
    exit 1
fi

# name of the directory where the output is written to
output="$experiment"-$(date +%Y%m%d%H%M%S)

if [[ -d $output ]]; then
    # since we add a date postfix to the name this shouldn't happen
    echo "Error: output directory already exists" >&2
    exit 2
fi

# create and change into output directory
mkdir "${output}"
cd "${output}"

# copy the output file to the directory and replace the variables
mkdir output
output_file=output/$experiment.txt
cp ../nnpdf31_proc/output/$experiment.txt "$output_file"
sed -i "s/@OUTPUT@/$experiment/g" "$output_file"

# create output folder
python2 "${mg5amc}" "$output_file" |& tee output.log

# copy patches if there are any
if [[ -d ../nnpdf31_proc/patches/$experiment ]]; then
    cp -r ../nnpdf31_proc/patches/$experiment/* "${experiment}"/
fi

# enforce proper analysis
cp ../nnpdf31_proc/analyses/$experiment.f "${experiment}"/FixedOrderAnalysis
sed -i "s/analysis_HwU_template/$experiment/g" "${experiment}"/Cards/FO_analyse_card.dat

# copy the launch file to the directory and replace the variables
mkdir launch
launch_file=launch/$experiment.txt
cp ../nnpdf31_proc/launch/$experiment.txt "$launch_file"
sed -i "s/@OUTPUT@/$experiment/g" "$launch_file"

# TODO: write a list with variables that should be replaced in the launch file; for the time being
# we create the file here, but in the future it should be read from the theory database
cat > variables.txt <<EOF
MT 172.5
MZ 91.176
YMT 172.5
EOF

# replace the variables with their values
sed -f <(sed -E 's|(.*) (.*)|s/@\1@/\2/|g' variables.txt) -i "$launch_file"

# remove the variables file
rm variables.txt

# launch run
python2 "${mg5amc}" "$launch_file" |& tee launch.log

# TODO: the following assumes that all observables belong to the same distribution

# merge the final bins
"${merge_bins}" ${experiment}.root ${experiment}/Events/run_02*/amcblast_obs_*.root

# find out which PDF set was used to generate the predictions
pdfstring=$(grep "set lhaid" "${launch_file}" | sed 's/set lhaid \([0-9]\+\)/\1/')

# (re-)produce predictions
"${applcheck}" ${pdfstring} ${experiment}.root &> applcheck.log

# extract the numerical results from mg5_aMC
sed -e '/^  [+-]/!d' \
    -e 's/^  [+-][0-9].[0-9]\+e[+-][0-9]\+   [+-][0-9].[0-9]\+e[+-][0-9]\+   [+]*\([0-9].[0-9]\+e[+-][0-9]\+\)   [+]*\([0-9].[0-9]\+e[+-][0-9]\+\)$/\1 \2/' \
    ${experiment}/Events/run_02*/MADatNLO.HwU > results.mg5_aMC

# extract the numerical results from the APPLgrid
sed -e '1,/all bins:/d' \
    -e '/sum:/,$d' \
    -e '/^$/d' \
    -e 's/^ bin #[ 0-9]*: [^o]*or //' \
    -e 's/ \[pb\]$//' applcheck.log > results.applgrid

# compare the results from the APPLgrid and from mg5_aMC
paste -d ' ' results.applgrid results.mg5_aMC | \
    awk 'function abs(x) { return x < 0.0 ? -x : x; }
         BEGIN { print "-----------------------------------------------------------"
                 print "   APPLgrid       mg5_aMC   mg5_aMC unc.  sigmas  per cent"
                 print "-----------------------------------------------------------" }
         { printf "% e % e %e %7.2f %7.2f%%\n", $1, $2, $3, abs($1-$2)/$3, abs($1-$2)/$2*100 }' | \
    tee results.log

rm results.mg5_aMC results.applgrid
