#!/bin/bash
pineappl write --remap '0,0.75,1.5,2.5;0,85,175,285,390,1000|0,85,175,285,1000|0,85,175,300,1000' "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
pineappl write --remap-norm 0.5  "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
