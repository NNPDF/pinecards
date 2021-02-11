#!/bin/bash

pineappl remap "${GRID}" "${NEW_GRID}" '116,150,200,300,500,1500;0,0.2,0.4,0.6,0.8,1,1.2,1.4,1.6,1.8,2,2.2,2.4|||0,0.4,0.8,1.2,1.6,2,2.4|' --ignore_obs_norm 1
mv "${GRID}".tmp "${GRID}"
