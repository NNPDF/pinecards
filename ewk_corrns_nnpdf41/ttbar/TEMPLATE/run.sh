#!/bin/bash

./../../../MG5_aMC_v3_6_7/bin/mg5_aMC output.txt
cp TEMPLATE.f TEMPLATE/FixedOrderAnalysis/
sed -i 's/analysis_HwU_template/TEMPLATE/g' TEMPLATE/Cards/FO_analyse_card.dat
cp setscales.f TEMPLATE/SubProcesses
./../../../MG5_aMC_v3_6_7/bin/mg5_aMC launch.txt
