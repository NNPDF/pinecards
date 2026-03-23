#!/bin/bash
pineappl write --remap '0,700,970,3000;0,0.46,0.91,1.55,2.50|0,0.33,0.62,0.95,2.50|0,0.4,0.79,2.5' "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
