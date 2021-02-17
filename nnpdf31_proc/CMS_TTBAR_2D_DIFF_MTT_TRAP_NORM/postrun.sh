#!/bin/bash

pineappl remap "${GRID}" "${GRID}".tmp '340,400,500,650,1500;0,0.35,0.85,1.45,2.5' --ignore_obs_norm 1
mv "${GRID}".tmp "${GRID}"
