#!/bin/bash

set -e

for i in $(find nnpdf31_proc/analyses -name *.f); do
    name=${i##*/}
    nameset=${name%%.f}

    ./run.sh ${nameset}
done
