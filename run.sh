#!/bin/bash

if [ -z "$*" ]; then
    echo "Usage: ./run.sh [nameexp]"
    exit
fi

experiment="$1"

cd 3.0.2/bin

#Create output folder
echo "Creating output folder"
python2 mg5_aMC ../../nnpdf31_proc/output/$experiment.txt

#Enforce proper analysis
echo "Copying the relevant analysis"
cp ../../nnpdf31_proc/analyses/$experiment.f $experiment/FixedOrderAnalysis
sed -i "s/ analysis_HwU_template/$experiment/g" $experiment/Cards/FO_analyse_card.dat

#Launch run
echo "Launch mg5_aMC"
python2 mg5_aMC ../../nnpdf31_proc/launch/$experiment.txt

cd ../../
