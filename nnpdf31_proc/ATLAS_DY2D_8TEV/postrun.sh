#!/bin/bash

NEW_GRID="${GRID%.lz4}"

# write the correct bin limits
pineappl remap "${GRID}" "${NEW_GRID}" '116,150,200,300,500,1500;0,0.2,0.4,0.6,0.8,1,1.2,1.4,1.6,1.8,2,2.2,2.4|||0,0.4,0.8,1.2,1.6,2,2.4|' --ignore_obs_norm 1
# compress the new grid
lz4 -9 -f "${NEW_GRID}"
# remove the uncompressed grid
rm "${NEW_GRID}"
