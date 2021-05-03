#!/bin/bash

common_substring() {
    # from https://stackoverflow.com/a/17475354/812178
    printf "%s\n" "$@" | sed -e 'N;s/^\(.*\).*\n\1.*$/\1\n\1/;D'
}

# exit script at the first sign of an error
set -o errexit

# the following exits if undeclared variables are used
set -o nounset

# exit if some program in a pipeline fails
set -o pipefail

array=( $(for i in "$@"; do basename $i; done) )
grid="$(common_substring "${array[@]}")"
grid="${grid%_}".pineappl

# merge grids
pineappl merge "${grid}" "$@"

# get all keys, possibly ones that are exclusive to a single grid
keys=( $(for i in "$@"; do
    pineappl info --keys $i
done | sort -u) )

grids=( "$@" )
echo "${grids[@]}"

# extract the results from each grid
for (( i=0; i != ${#grids[@]}; i++ )); do
    echo $i
    pineappl info "${grids[$i]}" --get results | \
        sed '/^ [0-9].*/!d' | \
        sed 's/^ 0.000000e+00  0.000000e+00   0.000   0.000   0.0000   0.0000   0.0000$//' \
        > tmp.results.$i
done

# extract the header from the first grid
pineappl info "${grids[0]}" --get results | sed -e '/^ [0-9]/d' -e '$ d' > tmp.results

# paste the results together and remove leading spaces
paste -d ' ' tmp.results.* | sed 's/^ */ /' >> tmp.results
rm tmp.results.*

# set the results metadata in the new grid
pineappl set "${grid}" "${grid}".tmp --entry_from_file results tmp.results
mv "${grid}".tmp "${grid}"
rm tmp.results

for key in "${keys[@]}"; do
    # skip results
    if [[ ${key} == results ]]; then
        continue
    fi

    lhs=$(pineappl info "${grid}" --get "${key}")
    for g in "$@"; do
        rhs=$(pineappl info "${g}" --get "${key}")
        if ! diff <(echo "${lhs}") <(echo "${rhs}") >/dev/null; then
            echo ${key} differs
        fi
    done
done

lz4 -9 "${grid}"
rm "${grid}"
