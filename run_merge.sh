#!/bin/bash

check_args_and_cd_output() {
    # exit script at the first sign of an error
    set -o errexit

    # the following exits if undeclared variables are used
    set -o nounset

    # exit if some program in a pipeline fails
    set -o pipefail

    # check arguments of the script
    if (( $# != 1 )); then
        exit 1
    fi

    # name of the dataset
    dataset="${1%-[0-9]*}"

    mg5amc=$(which mg5_aMC 2> /dev/null || true)

    if [[ ! -x ${mg5amc} ]]; then
        echo "The binary \`mg5_aMC\` wasn't found. Please adjust your PATH variable" >&2
        exit 1
    fi

    pineappl=$(which pineappl 2> /dev/null || true)

    if [[ ! -x ${pineappl} ]]; then
        echo "The binary \`pineappl\` wasn't found. Please adjust your PATH variable" >&2
        exit 1
    fi

    # name of the directory where the output is written to
    output="$1"

    if [[ ! -d $output ]]; then
        # since we add a date postfix to the name this shouldn't happen
        echo "Error: output directory doesn't exists" >&2
        exit 1
    fi

    cd "${output}"
}

main() {
    launch_file=launch.txt

    # TODO: the following assumes that all observables belong to the same distribution

    grid="${dataset}".pineappl
    rm -f "${grid}"

    # merge the final bins
    "${pineappl}" merge "${grid}" $(ls -v "${dataset}"/Events/run_01*/amcblast_obs_*.root)

    lz4=$(which lz4 2> /dev/null || true)

    # compress the grid with `lz4` if it's available
    if [[ -x ${lz4} ]]; then
        lz4 -9 "${grid}"
        rm "${grid}"
        grid="${grid}.lz4"
    fi

    # find out which PDF set was used to generate the predictions
    pdfstring=$(grep "set lhaid" "${launch_file}" | sed 's/set lhaid \([0-9]\+\)/\1/')

    # (re-)produce predictions
    "${pineappl}" convolute "${grid}" "${pdfstring}" --scales 9 > pineappl.convolute
    "${pineappl}" orders "${grid}" "${pdfstring}" --absolute > pineappl.orders
    "${pineappl}" pdf_uncertainty "${grid}" "${pdfstring}" > pineappl.pdf_uncertainty

    # extract the numerical results from mg5_aMC
    sed '/^  [+-]/!d' "${dataset}"/Events/run_01*/MADatNLO.HwU > results.mg5_aMC

    # extract the integrated results from the PineAPPL grid
    cat pineappl.convolute | head -n -2 | tail -n +5 | awk '{ print $5 }' > results.grid

    # compare the results from the grid and from mg5_aMC
    paste -d ' ' results.grid results.mg5_aMC | awk \
        'function abs(x) { return x < 0.0 ? -x : x; }
         BEGIN { print "----------------------------------------------------------------------------------------"
                 print "   PineAPPL       mg5_aMC   mg5_aMC unc.   sigmas   per mille   9-point scale var. (MC)"
                 print "----------------------------------------------------------------------------------------" }
         { printf "% e % e %e %8.3f %10.4f % e % e\n",
                  $1, $4, $5, $5 != 0.0 ? abs($1-$4)/$5 : 0.0, $4 != 0.0 ? abs($1-$4)/$4*1000 : 0.0, $7, $8 }' | \
        tee results.log

    rm results.mg5_aMC results.grid
}

check_args_and_cd_output "$@"

# record the time and write it to stdout and `time.log`
{ { time { main 2>&3; } } 2>time.log; } 3>&2
cat time.log

echo "Output stored in ${output}"
