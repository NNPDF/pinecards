#!/bin/bash

NEW_GRID="${GRID%.lz4}"

# divide the grid values by two
pineappl merge "${NEW_GRID}" "${GRID}" --scale 0.5
# compress the new grid
lz4 -9 -f "${NEW_GRID}"
