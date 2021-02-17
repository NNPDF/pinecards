#!/bin/bash

pineappl remap "${GRID}" "${GRID}".tmp '66,116,150;1.2,1.4,1.6,1.8,2,2.2,2.4,2.8,3.2,3.6|1.2,1.6,2,2.4,2.8,3.2,3.6' --ignore_obs_norm 1
mv "${GRID}".tmp "${GRID}"
