#!/bin/bash

if [[ ! -f $1 || ! -f $2 ]]; then
    echo "usage: <old mg5_aMC directory> <new mg5_aMC directory>"
    exit 1
fi

set -e

CWD="$(pwd)"
ONE="$1"
TWO="$2"

# see https://unix.stackexchange.com/a/84980 for an explanation
tmpdir=$(mktemp -d 2>/dev/null || mktemp -d -t 'mytmpdir')
cd "${tmpdir}"

# create runcard for MG5_aMC
cat > runcard <<EOF
set auto_update 0
generate p p > W+ [QCD]
output amcfast_test

launch -i
set automatic_html_opening False
shell sed -i 's/FO_ANALYSIS_FORMAT = HwU/FO_ANALYSIS_FORMAT = topdrawer/' amcfast_test/Cards/FO_analyse_card.dat
shell sed -i 's/FO_ANALYSE = analysis_HwU_template.o/FO_ANALYSE = analysis_td_template.o/' amcfast_test/Cards/FO_analyse_card.dat
generate_events NLO
done
set run_card iappl 1
set run_card pdlabel lhapdf
set run_card lhaid 229800
set run_card iseed 2
done
generate_events NLO --appl_start_grid=run_01
done
set run_card req_acc_FO 0.001
set run_card iappl 2
done
quit
quit
EOF

# run first version of MG5_aMC
"${ONE}" -f runcard #-l DEBUG
cp amcfast_test/Events/run_02/aMCfast_obs_0.root "${CWD}"/ONE.root

# remove the directory, because it will be used by the second call to mg5_aMC
rm -rf amcfast_test

# run second version of MG5_aMC
"${TWO}" -f runcard #-l DEBUG
cp amcfast_test/Events/run_02/amcblast_obs_0.root "${CWD}"/TWO.root

# exit the temporary directory and remove it
cd && rm -rf "${tmpdir}"
