#!/bin/bash

pineappl remap "${GRID}" "${GRID}".tmp '0,0.35,0.85,1.45,2.5;80,150,250,600' --ignore_obs_norm 1
mv "${GRID}".tmp "${GRID}"
