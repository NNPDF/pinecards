#!/bin/bash
pineappl write --remap '0,0.12,0.24,0.36,0.49,0.62,0.76,0.91,1.06,1.21,1.39,1.59,2.40' "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
