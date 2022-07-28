#!/bin/bash

pineappl remap "${GRID}" "${GRID}".tmp '340,400,500,650,1500;0,0.35,0.75,1.15,2.5'
mv "${GRID}".tmp "${GRID}"
