#!/bin/bash
pineappl write --remap-norm 0.5  "${GRID}" "${GRID}".tmp
mv "${GRID}".tmp "${GRID}"
