#!/bin/bash
pineappl write --remap '340,400,500,650,1500;0,0.35,0.85,1.45,2.50|0,0.35,0.85,1.45,2.50|0,0.35,0.85,1.45,2.50|0,0.35,0.85,1.45,2.50' "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
pineappl write --remap-norm 0.5  "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
