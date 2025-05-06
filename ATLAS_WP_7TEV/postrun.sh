#!/bin/bash

pineappl write "${GRID}" "${GRID}".tmp  --scale 0.5
mv "${GRID}".tmp "${GRID}"
