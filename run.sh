#!/bin/bash

# exit script at the first sign of an error
set -o errexit

# the following exits if undeclared variables are used
set -o nounset

if [[ $# != 1 ]] ; then
    echo "Usage: ./run.sh [nameset]"
    echo "  The following namesets are available:"

    for i in $(find nnpdf31_proc/analyses -name *.f); do
        name=${i##*/}
        nameset=${name%%.f}
        echo "  - ${nameset}"
    done

    exit 2
fi

experiment="$1"
output="$experiment"-$(date +%Y%m%d%H%M%S)

if [ -d "$output" ]; then
    # since we add a date postfix to the name this shouldn't happen
    echo "Error: output directory already exists"
    exit 2
fi

mg5amc=$(which mg5_aMC)

if [[ ! -x "${mg5amc}" ]]; then
    echo "The binary \`mg5_aMC\' wasn't found. Please adjust your PATH variable."
    exit 1
fi

# create a temporary directory and delete it when exiting
tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT

mkdir "$tmpdir"/output
output_file="$tmpdir"/output/$experiment.txt
cp nnpdf31_proc/output/$experiment.txt "$output_file"
sed -i "s/@OUTPUT@/$output/g" "$output_file"

# create output folder
python2 "${mg5amc}" "$output_file"

# copy patches if there are any
if [[ -d nnpdf31_proc/patches/$experiment ]]; then
    echo "Copying patches"
    cp -r nnpdf31_proc/patches/$experiment/* "$output"/
fi

# enforce proper analysis
echo "Copying the relevant analysis"
cp nnpdf31_proc/analyses/$experiment.f "$output"/FixedOrderAnalysis
sed -i "s/analysis_HwU_template/$experiment/g" "$output"/Cards/FO_analyse_card.dat

mkdir "$tmpdir"/launch
launch_file="$tmpdir"/launch/$experiment.txt
cp nnpdf31_proc/launch/$experiment.txt "$launch_file"
sed -i "s/@OUTPUT@/$output/g" "$launch_file"

# launch run
echo "Launch mg5_aMC"
python2 "${mg5amc}" "$launch_file"
