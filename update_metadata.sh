#!/bin/bash

# exit script at the first sign of an error
set -o errexit

# the following exits if undeclared variables are used
set -o nounset

# exit if some program in a pipeline fails
set -o pipefail

for i in $(find -maxdepth 1 -mindepth 1 -type d); do
    dataset=${i#./}
    dataset=${dataset%-[0-9]*}
    grid=$i/${dataset}.pineappl.lz4

    if [[ -f ${grid} ]]; then
        export grid
        export metadata=nnpdf31_proc/${dataset}/metadata.txt

        if [[ ! -f ${metadata} ]]; then
            echo "Skipping ${grid#./}, couldn't find metadata (is the git repository on the right branch?)"
            continue
        else
            echo "Updating ${grid#./}"
        fi

        eval $(awk -F= "BEGIN { printf \"pineappl set ${grid} ${grid}.tmp \" } { printf \"--entry %s '%s' \", \$1, \$2 }" ${metadata})

        lz4 -9 ${i}/${dataset}.pineappl.lz4.tmp 2>/dev/null
        rm ${i}/${dataset}.pineappl.lz4.tmp
        mv ${i}/${dataset}.pineappl.lz4.tmp.lz4 ${grid}
    fi
done
