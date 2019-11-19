#!/bin/bash

pdfset="MSTW2008nlo68cl"
path="../2.6.5/bin/ppz/Events/run_03/"
grid="aMCfast_obs_0.root"

./applcheck ${pdfset} ${path}${grid}
