#!/bin/bash

set -e

if [[ ! -f $1 ]]; then
    echo "usage: <applgriddiff> <reference-dir> <trunk-dir>"
    exit 1
fi

root=$(git rev-parse --show-toplevel)
diff="$1"
dir1="$2"
dir2="$3"

for i in "${root}"/refproc/*; do
    process=$(basename $i)
    for j in "${dir1}"/${process}/*.root; do
        file=$(basename $j)
        file1="${dir1}/${process}/${file}"
        file2="${dir2}/${process}/amcblast${file#aMCfast}"

        echo "# comparing \`${file1}\` against \`${file2}\`"
        "${diff}" "${file1}" "${file2}" || true
    done
done
