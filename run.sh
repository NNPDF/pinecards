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
        echo "Usage: ./run.sh [dataset]" >&2
        echo "  The following datasets are available:" >&2

        for i in $(ls -d nnpdf31_proc/*); do
            echo "  - ${i##*/}" >&2
        done

        exit 1
    fi

    # name of the dataset
    dataset="$1"

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
    output="${dataset}"-$(date +%Y%m%d%H%M%S)

    if [[ -d $output ]]; then
        # since we add a date postfix to the name this shouldn't happen
        echo "Error: output directory already exists" >&2
        exit 1
    fi

    mkdir "${output}"
    cd "${output}"
}

main() {
    # copy the output file to the directory and replace the variables
    output_file=output.txt
    cp ../nnpdf31_proc/"${dataset}"/output.txt "${output_file}"
    sed -i "s/@OUTPUT@/${dataset}/g" "${output_file}"

    # create output folder
    python2 "${mg5amc}" "${output_file}" |& tee output.log

    # copy patches if there are any
    for i in $(find ../nnpdf31_proc/"${dataset}" -name '*.patch'); do
        patch -p1 -d "${dataset}" < $i
    done

    # enforce proper analysis
    cp ../nnpdf31_proc/"${dataset}"/analysis.f "${dataset}"/FixedOrderAnalysis/"${dataset}".f
    sed -i "s/analysis_HwU_template/${dataset}/g" "${dataset}"/Cards/FO_analyse_card.dat

    # copy the launch file to the directory and replace the variables
    launch_file=launch.txt
    cp ../nnpdf31_proc/"${dataset}"/launch.txt "${launch_file}"
    sed -i "s/@OUTPUT@/${dataset}/g" "${launch_file}"

    # TODO: write a list with variables that should be replaced in the launch file; for the time
    # being we create the file here, but in the future it should be read from the theory database
    cat > variables.txt <<EOF
MT 172.5
MW 80.419
MZ 91.176
YMT 172.5
EOF

    # replace the variables with their values
    sed -f <(sed -E 's|(.*) (.*)|s/@\1@/\2/|g' variables.txt) -i "${launch_file}"

    # perform simple arithmetic on lines containing 'set' and '=' and arithmetic operators
    awk '{
        if (match($0, /set[^=]+=[^+-]+[+-]/)) {
            a = ""
            set_at = 0
            for (i=1;i<=NF;++i) {
                if ($i == "set") {
                    set_at = i
                }

                if ((set_at == 0) || (i < set_at + 3)) {
                    printf $i " "
                } else {
                    a = a $i
                }
            }
            system("echo " a " | bc -l")
        } else { print $0 }
    }' "${launch_file}" > "${launch_file}".arithmetic

    # replace launch file
    mv "${launch_file}".arithmetic "${launch_file}"

    # remove the variables file
    rm variables.txt

    # parse launch file for user-defined cuts
    user_defined_cuts=$(grep '^#user_defined_cut' launch.txt || true)

    # if there are user-defined cuts, implement them
    if [[ -n ${user_defined_cuts[@]} ]]; then
        user_defined_cuts=( $(echo "${user_defined_cuts[@]}" | grep -Eo '\w+[[:blank:]]+=[[:blank:]]+[+-]?[0-9]+([.][0-9]+)?') )
        ../run_implement_user_defined_cuts.py "${dataset}"/SubProcesses/cuts.f "${user_defined_cuts[@]}"
    fi

    # launch run
    python2 "${mg5amc}" "${launch_file}" |& tee launch.log

    # TODO: the following assumes that all observables belong to the same distribution

    grid="${dataset}".pineappl

    # merge the final bins
    "${pineappl}" merge "${grid}" $(ls -v "${dataset}"/Events/run_01*/amcblast_obs_*.pineappl)

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

    # if there is anything to do after the run, do it!
    if [[ -x ../nnpdf31_proc/"${dataset}"/postrun.sh ]]; then
        cp ../nnpdf31_proc/"${dataset}"/postrun.sh .
        GRID=$grid ./postrun.sh
    fi
}

check_args_and_cd_output "$@"

# record the time and write it to stdout and `time.log`
{ { time { main 2>&3; } } 2>time.log; } 3>&2
cat time.log

echo "Output stored in ${output}"
