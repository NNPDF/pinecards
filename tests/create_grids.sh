#!/bin/bash

set -e

if [[ ! -f $1 || ! -d $2 ]]; then
    echo "usage: <mg5_aMC> <processdir>"
    exit 1
fi

cwd="$(pwd)"
mg5="$1"

# points to process that should be checked
refprocdir="$(cd $2 && pwd)"

# see https://unix.stackexchange.com/a/84980 for an explanation
tmpdir=$(mktemp -d 2>/dev/null || mktemp -d -t 'mytmpdir')
cd "${tmpdir}"

analysis="${refprocdir}"/analysis_td_template.f
param="${refprocdir}"/param_card.dat
run="${refprocdir}"/run_card.dat
proc="${refprocdir}"/proc_card_mg5.dat

if [[ ! -f ${analysis} ]]; then
    echo "missing file: \`${analysis}\`"
    exit 1
fi

if [[ ! -f ${param} ]]; then
    echo "directory \`${refprocdir}\` does not contain a parameter file"
    exit 1
fi

if [[ ! -f ${run} ]]; then
    echo "directory \`${refprocdir}\` does not contain a run file"
    exit 1
fi

if [[ ! -f ${proc} ]]; then
    echo "directory \`${refprocdir}\` does not contain a process file"
    exit 1
fi

# extract output directory from process card
output=$(sed -n '/^[[:space:]]*output[[:space:]]*/{s/^[[:space:]]*output[[:space:]]//;p}' "${proc}")

# copy process card
cp "${proc}" proccard

# edit some details in the process card
cat >> proccard <<EOF
set auto_update 0
launch -i
set automatic_html_opening False
shell sed -i 's/FO_ANALYSIS_FORMAT = HwU/FO_ANALYSIS_FORMAT = topdrawer/' ${output}/Cards/FO_analyse_card.dat
shell sed -i 's/FO_ANALYSE = analysis_HwU_template.o/FO_ANALYSE = analysis_td_template.o/' ${output}/Cards/FO_analyse_card.dat
shell cp "${analysis}" ${output}/FixedOrderAnalysis/
shell cp "${param}" "${run}" ${output}/Cards/
generate_events NLO
done
set run_card iappl 1
#set run_card pdlabel lhapdf
#set run_card lhaid 229800
set run_card iseed 2
done
generate_events NLO --appl_start_grid=run_01
done
#set run_card req_acc_FO 0.001
set run_card iappl 2
done
quit
quit
EOF

# run MG5_aMC
"${mg5}" -f proccard -l CRITICAL
cp "${output}"/Events/run_02/*.root "${cwd}"/

# exit the temporary directory and remove it
cd "${cwd}" && rm -rf "${tmpdir}"
