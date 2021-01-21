#!/bin/bash

set -e

pineappl info --set initial_state_2 -2212 "${GRID}".tmp "${GRID}"
lz4 -9 "${GRID}".tmp
rm "${GRID}".tmp
mv "${GRID}".tmp.lz4 "${GRID%.lz4}".lz4
