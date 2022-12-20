#!/bin/bash

# factor 0.5 takes into account that LHCb has only half the hemisphere of a 4pi
# detector, while Madgraph uses the full hemisphere
pineappl merge "${GRID}".tmp "${GRID}" --scale 0.5
mv "${GRID}".tmp "${GRID}"
