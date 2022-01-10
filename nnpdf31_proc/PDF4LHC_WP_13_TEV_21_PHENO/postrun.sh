#!/bin/bash

pineappl remap "${GRID}" "${GRID}".tmp '1000,1500,2000,3000,4000,5000,7000'
mv "${GRID}".tmp "${GRID}"
