#!/bin/bash

if [ $# -lt 2 ] || [ $# -gt 2 ] ; then
  echo "Usage: ./run.sh [nameexp] [bin]"
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

cd 3.0.2/bin

#Create output folder
echo "Creating output folder"
python2 mg5_aMC ../../nnpdf31_proc/output/$experimentbin.txt

#Enforce proper analysis
echo "Copying the relevant analysis"
cp ../../nnpdf31_proc/analyses/$experiment/$experimentbin.f $experimentbin/FixedOrderAnalysis
sed -i "s/analysis_HwU_template/$experimentbin/g" $experimentbin/Cards/FO_analyse_card.dat

##Launch run
echo "Launch mg5_aMC"
python2 mg5_aMC ../../nnpdf31_proc/launch/$experimentbin.txt

cd ../../

#Cleanup
rm nnpdf31_proc/output/$experimentbin.txt
rm nnpdf31_proc/launch/$experimentbin.txt
