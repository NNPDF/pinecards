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

if [ -d $experiment ]; then
    echo "Error: output directory already exists"
    exit 2
fi

mg5amc=$(which mg5_aMC)

if [[ ! -x "${mg5amc}" ]]; then
    echo "The binary \`mg5_aMC\' wasn't found. Please adjust your PATH variable."
    exit 1
fi

#Create output folder
echo "Creating output folder"
python2 "${mg5amc}" nnpdf31_proc/output/$experiment.txt

# Copy patches if there are any
if [[ -d nnpdf31_proc/patches/$experiment ]]; then
    echo "Copying patches"
    cp -r nnpdf31_proc/patches/$experiment/* $experiment
fi

#Enforce proper analysis
echo "Copying the relevant analysis"
cp nnpdf31_proc/analyses/$experiment.f $experiment/FixedOrderAnalysis
sed -i "s/analysis_HwU_template/$experiment/g" $experiment/Cards/FO_analyse_card.dat

##Launch run
echo "Launch mg5_aMC"
python2 "${mg5amc}" nnpdf31_proc/launch/$experiment.txt
