#!/bin/bash

# exit script at the first sign of an error
set -e

if [ $# -lt 2 ] || [ $# -gt 2 ] ; then
    echo "Usage: ./run.sh [nameset] [binID]"
    echo "  The following combinations of namesets and binIDs are available:"

    for i in $(find nnpdf31_proc/analyses -name *.f); do
        name=${i##*/}
        nameset=${name%%_[0-9]*.f}
        binid=${name%.f}
        binid=${binid##*_}
        echo "  - ${nameset} ${binid}"
    done

    exit 2
fi

experiment="$1"
bin="$2"
experimentbin="$experiment""_""$bin"

#Copying dirs for binning purposes
cp nnpdf31_proc/output/$experiment.txt nnpdf31_proc/output/$experimentbin.txt
sed -i "s/$experiment/$experimentbin/g" nnpdf31_proc/output/$experimentbin.txt
cp nnpdf31_proc/launch/$experiment.txt nnpdf31_proc/launch/$experimentbin.txt
sed -i "s/$experiment/$experimentbin/g"	nnpdf31_proc/launch/$experimentbin.txt

mg5amc=$(which mg5_aMC)

if [[ ! -x "${mg5amc}" ]]; then
    echo "The binary \`mg5_aMC\' wasn't found. Please adjust your PATH variable."
    exit 1
fi

#Create output folder
echo "Creating output folder"
python2 "${mg5amc}" nnpdf31_proc/output/$experimentbin.txt

# Copy patches if there are any
if [[ -d nnpdf31_proc/patches/$experiment ]]; then
    echo "Copying patches"
    cp -r nnpdf31_proc/patches/$experiment/* $experimentbin/
fi

#Enforce proper analysis
echo "Copying the relevant analysis"
cp nnpdf31_proc/analyses/$experiment/$experimentbin.f $experimentbin/FixedOrderAnalysis
sed -i "s/analysis_HwU_template/$experimentbin/g" $experimentbin/Cards/FO_analyse_card.dat

##Launch run
echo "Launch mg5_aMC"
python2 "${mg5amc}" nnpdf31_proc/launch/$experimentbin.txt

#Cleanup
rm nnpdf31_proc/output/$experimentbin.txt
rm nnpdf31_proc/launch/$experimentbin.txt
