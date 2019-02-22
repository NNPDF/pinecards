#!/bin/bash

set -e

if [[ ! -f $1 ]]; then
    echo "usage: <mg5_aMC>"
    exit 1
fi

root=$(git rev-parse --show-toplevel)
mg5="$1"

for i in "${root}"/refproc/*; do
    dir=$(basename $i)

    if [[ -d ${dir} ]]; then
        echo "directory \`${dir}\` already exists"
        exit 1
    fi

    mkdir ${dir}
    cd ${dir}
    "${root}"/tests/create_grids.sh "${mg5}" "${i}"
    cd ..
done
