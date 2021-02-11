#!/bin/bash

pineappl merge "${GRID}".tmp "${GRID}" --scale 0.5
mv "${GRID}".tmp "${GRID}"
