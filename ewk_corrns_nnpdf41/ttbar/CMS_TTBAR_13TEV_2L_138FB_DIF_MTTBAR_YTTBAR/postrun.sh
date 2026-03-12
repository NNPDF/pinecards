#!/bin/bash
pineappl write --remap '300,400,500,650,1500;0,0.35,0.75,1.15,2.5|0,0.35,0.75,1.15,2.5|0,0.35,0.75,1.15,2.5|0,0.35,0.75,1.15,2.5' "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
pineappl write --remap-norm 0.5  "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
