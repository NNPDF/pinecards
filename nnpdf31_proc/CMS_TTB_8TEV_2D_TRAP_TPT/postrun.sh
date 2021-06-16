#!/bin/bash

pineappl remap "${GRID}" "${GRID}".tmp '0,0.35,0.85,1.45,2.5;0,80,150,250,600'
mv "${GRID}".tmp "${GRID}"
