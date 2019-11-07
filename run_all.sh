#!/bin/bash

set -e

for i in $(find nnpdf31_proc/analyses -name *.f); do
    name=${i##*/}
    nameset=${name%%_[0-9]*.f}
    binid=${name%.f}
    binid=${binid##*_}

    ./run.sh ${nameset} ${binid} | tee log-${nameset}-${binid}.txt
done
