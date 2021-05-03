#!/bin/bash

# exit script at the first sign of an error
set -o errexit

# the following exits if undeclared variables are used
set -o nounset

# exit if some program in a pipeline fails
set -o pipefail

# merge grids
pineappl merge tmp.pineappl "$@"

# compress result
lz4 -9 tmp.pineappl
rm tmp.pineappl

# from https://stackoverflow.com/a/17475354/812178
grid=$(printf "%s\n" "$@" | sed -e 'N;s/^\(.*\).*\n\1.*$/\1\n\1/;D')

# rename result
mv tmp.pineappl.lz4 "${grid}".pineappl.lz4
