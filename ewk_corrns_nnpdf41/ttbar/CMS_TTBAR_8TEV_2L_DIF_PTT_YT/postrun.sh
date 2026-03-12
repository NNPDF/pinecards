#!/bin/bash
pineappl write --remap '0,0.35,0.80,1.45,2.50;0,80,150,250,600|0,80,150,250,600|0,80,150,250,600|0,80,150,250,600' "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
pineappl write --remap-norm 0.5  "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
