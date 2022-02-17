#!/bin/bash

pineappl remap "${GRID}" "${GRID}".tmp '0,0.4,0.8,1.2,1.6,2;0,20,40,60,80,100,120,140,170,200,1000'
mv "${GRID}".tmp "${GRID}"
