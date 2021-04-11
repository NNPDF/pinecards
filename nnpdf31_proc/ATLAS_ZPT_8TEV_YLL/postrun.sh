#!/bin/bash

pineappl remap "${GRID}" "${GRID}".tmp '0,0.4,0.8,1.2,1.6,2,2.4;0,2,4,6,8,10,13,16,20,25,30,37,45,55,65,75,85,105,150,200,900' --ignore_obs_norm 1
mv "${GRID}".tmp "${GRID}"
