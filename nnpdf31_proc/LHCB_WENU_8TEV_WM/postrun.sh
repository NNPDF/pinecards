#!/bin/bash

# divide the grid values by two
pineappl merge "${GRID}".tmp "${GRID}" --scale 0.5
mv "${GRID}".tmp "${GRID}"
