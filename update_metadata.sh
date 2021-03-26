#!/bin/bash

# exit script at the first sign of an error
set -o errexit

# the following exits if undeclared variables are used
set -o nounset

# exit if some program in a pipeline fails
set -o pipefail

for i in $(find -name '*.pineappl.lz4'); do
    dataset=$(basename $i)
    dataset=${dataset%.pineappl.lz4}

    export grid=${i}
    export metadata=nnpdf31_proc/${dataset}/metadata.txt

    if [[ ! -f ${metadata} ]]; then
        echo "Skipping ${i#./}, couldn't find metadata (is the git repository on the right branch?)"
        continue
    else
        echo "Updating ${i#./}"
    fi

    eval $(awk -F= "BEGIN { printf \"pineappl set ${grid} ${grid}.tmp \" } { printf \"--entry %s '%s' \", \$1, \$2 }" ${metadata})

    lz4 -9 ${i}.tmp 2>/dev/null
    rm ${i}.tmp
    mv ${i}.tmp.lz4 ${i}
done
