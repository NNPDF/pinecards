#!/bin/bash

pineappl remap "${GRID}" "${GRID}".tmp '340,400,500,650,1500;0.35,0.75,1.15,2.5' --ignore_obs_norm 1
mv "${GRID}".tmp "${GRID}"
