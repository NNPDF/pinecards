#!/bin/bash

prefix=$(pwd)/.prefix

export LC_ALL=C

yesno() {
    echo -n "$@" "[Y/n]"
    read -r reply

    if [[ -z $reply ]]; then
        reply=Y
    fi

    case "${reply}" in
    Yes|yes|Y|y) return 0;;
    No|no|N|n) return 1;;
    *) echo "I didn't understand your reply '${reply}'"; yesno "$@";;
    esac
}

install_mg5amc() {(
    mkdir -p "${prefix}"

    brz=$(which brz 2> /dev/null || true)
    bzr=$(which bzr 2> /dev/null || true)
    pip=$(which pip 2> /dev/null || true)

    repo=lp:~maddevelopers/mg5amcnlo/3.1.2

    if [[ -x ${pip} ]] && [[ ! -x ${brz} ]] && [[ ! -x ${bzr} ]]; then
        pyver=$(python --version | cut -d' ' -f 2 | cut -d. -f1,2)
        export PATH="${prefix}"/bin:${PATH}
        export PYTHONPATH="${prefix}"/lib/python${pyver}/site-packages
        "${pip}" install --prefix "${prefix}" breezy
        brz="${prefix}"/bin/brz
    fi

    if [[ -x ${brz} ]]; then
        "${brz}" branch "${repo}" "${prefix}"/mg5amc
    elif [[ -x ${bzr} ]]; then
        "${bzr}" branch "${repo}" "${prefix}"/mg5amc
    else
        echo "Couldn't install Madgraph5_aMC@NLO" >&2
        exit 1
    fi

    if "${pip}" show six 2>&1 | grep 'Package(s) not found' > /dev/null; then
        "${pip}" install --prefix "${prefix}" six
    fi

    # in case we're using python3, we need to convert the model file
    "${prefix}"/mg5amc/bin/mg5_aMC <<EOF
set auto_convert_model True
import model loop_qcd_qed_sm_Gmu
quit
EOF
)}

install_pineappl() {(
    mkdir -p "${prefix}"

    cargo=$(which cargo 2> /dev/null || true)
    git=$(which git 2> /dev/null)

    repo=https://github.com/N3PDF/pineappl.git

    if [[ ! -x ${cargo} ]]; then
        export CARGO_HOME=${prefix}/cargo
        curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs > /tmp/rustup-init
        bash /tmp/rustup-init --profile minimal --no-modify-path -y
        export PATH=${prefix}/cargo/bin:${PATH}
        cargo="${prefix}"/cargo/bin/cargo
    elif [[ -d ${prefix}/cargo ]]; then
        export CARGO_HOME=${prefix}/cargo
    fi

    if [[ -d ${prefix}/pineappl ]]; then
        "${git}" pull
    else
        "${git}" clone ${repo} "${prefix}"/pineappl
    fi

    "${cargo}" install --force cargo-c

    pushd . > /dev/null
    cd "${prefix}"/pineappl
    "${cargo}" cinstall --release --prefix "${prefix}" --manifest-path=pineappl_capi/Cargo.toml
    "${cargo}" install --path pineappl_cli --root "${prefix}"
    popd > /dev/null
)}

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

        for i in nnpdf31_proc/*; do
            echo "  - ${i##*/}" >&2
        done

        exit 1
    fi

    # name of the dataset
    dataset="$1"

    pkg_config=$(which pkg-config 2> /dev/null || true)

    if [[ ! -x ${pkg_config} ]]; then
        echo "The binary \`pkg-config\` wasn't found. Please install it" >&2
        exit 1
    fi

    # if we've installed dependencies set the correct paths
    if [[ -d ${prefix} ]]; then
        pyver=$(python --version | cut -d' ' -f 2 | cut -d. -f1,2)
        export PYTHONPATH="${prefix}"/lib/python${pyver}/site-packages:${PYTHONPATH:-}
        export PATH=${prefix}/mg5amc/bin:${prefix}/bin:${PATH:-}
        export LD_LIBRARY_PATH=${prefix}/lib:${LD_LIBRARY_PATH:-}
        export PKG_CONFIG_PATH=${prefix}/lib/pkgconfig:${PKG_CONFIG_PATH:-}
    fi

    install_mg5amc=
    install_pineappl=

    if ! which mg5_aMC > /dev/null 2>&1; then
        install_mg5amc=yes
        echo "Madgraph5_aMC@NLO wasn't found"
    fi

    if ! "${pkg_config}" pineappl_capi; then
        install_pineappl=yes
        echo "PineAPPL wasn't found"
    elif ! which pineappl > /dev/null 2>&1; then
        install_pineappl=yes
        echo "PineAPPL wasn't found"
    fi

    if [[ -n ${install_mg5amc}${install_pineappl} ]]; then
        if yesno "Do you want to install the missing dependencies (into \`.prefix\`)?"; then
            if [[ -n ${install_mg5amc} ]]; then
                install_mg5amc
            fi
            if [[ -n ${install_pineappl} ]]; then
                install_pineappl
            fi

            pyver=$(python --version | cut -d' ' -f 2 | cut -d. -f1,2)
            export PYTHONPATH="${prefix}"/lib/python${pyver}/site-packages:${PYTHONPATH:-}
            export PATH=${prefix}/mg5amc/bin:${prefix}/bin:${PATH:-}
            export LD_LIBRARY_PATH=${prefix}/lib:${LD_LIBRARY_PATH:-}
            export PKG_CONFIG_PATH=${prefix}/lib/pkgconfig:${PKG_CONFIG_PATH:-}
        else
            exit 1
        fi
    fi

    mg5amc=$(which mg5_aMC 2> /dev/null || true)

    if [[ ! -x ${mg5amc} ]]; then
        echo "The binary \`mg5_aMC\` wasn't found. Something went wrong" >&2
        exit 1
    fi

    pineappl=$(which pineappl 2> /dev/null || true)

    if [[ ! -x ${pineappl} ]]; then
        echo "The binary \`pineappl\` wasn't found. Something went wrong" >&2
        exit 1
    fi
}

output_and_launch() {
    # name of the directory where the output is written to
    output="${dataset}"-$(date +%Y%m%d%H%M%S)

    if [[ -d $output ]]; then
        # since we add a date postfix to the name this shouldn't happen
        echo "Error: output directory already exists" >&2
        exit 1
    fi

    mkdir "${output}"
    cd "${output}"

    # copy the output file to the directory and replace the variables
    output_file=output.txt
    cp ../nnpdf31_proc/"${dataset}"/output.txt "${output_file}"
    sed -i "s/@OUTPUT@/${dataset}/g" "${output_file}"

    # create output folder
    "${mg5amc}" "${output_file}" |& tee output.log

    # copy patches if there are any; use xargs to properly signal failures
    cd "${dataset}"
    find ../../nnpdf31_proc/"${dataset}" -name '*.patch' -print0 | \
        xargs -0 -I file sh -c 'patch -p1 < file'
    cd -

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
GF 1.1663787e-5
MH 125.0
MT 172.5
MW 80.352
MZ 91.1535
WH 4.07468e-3
WT 1.37758
WW 2.084
WZ 2.4943
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
    if [[ -n ${user_defined_cuts} ]]; then
        cuts=()
        mapfile -d ' ' -t cuts < <(
            echo "${user_defined_cuts[@]}" | \
            grep -Eo '\w+[[:blank:]]+=[[:blank:]]+([+-]?[0-9]+([.][0-9]+)?|True|False)' | \
            tr '\n' ' '
        )
        ../run_implement_user_defined_cuts.py "${dataset}"/SubProcesses/cuts.f "${cuts[@]}"
    fi

    # parse launch file for user-defined minimum tau
    user_defined_tau_min=$(grep '^#user_defined_tau_min' launch.txt || true)

    # if there is one, implement it
    if [[ -n ${user_defined_tau_min} ]]; then
        user_defined_tau_min=( ${user_defined_tau_min} )
        sed "s/@TAU_MIN@/${user_defined_tau_min[1]}d0/" ../patches/set_tau_min.patch > \
            set_tau_min.patch
        patch -p1 -d "${dataset}" < set_tau_min.patch
    fi

    # parse launch file for other patches
    enable_patches=$(grep '^#enable_patch' launch.txt || true)

    if [[ -n ${enable_patches} ]]; then
        while IFS= read -r line; do
            patch=( $line )

            patch -p1 -d "${dataset}" < ../patches/"${patch[1]}".patch
        done < <(printf '%s\n' "${enable_patches}")
    fi

    # launch run
    "${mg5amc}" "${launch_file}" |& tee launch.log
}

merge() {
    # TODO: the following assumes that all observables belong to the same distribution

    grid="${dataset}".pineappl.lz4

    # sort the file we want to merge into an array properly (1 2 3 ... 10 11 instead of 1 10 11 ...)
    merge=()
    mapfile -t merge < <(printf "%s\n" "${dataset}"/Events/run_01*/amcblast_obs_*.pineappl | sort -V)

    # merge the final bins
    "${pineappl}" merge "${grid}" "${merge[@]}"

    # optimize the grids
    "${pineappl}" write --optimize "${grid}" "${grid}".tmp
    mv "${grid}".tmp "${grid}"

    # add metadata
    runcard=( "${dataset}"/Events/run_01*/run_01*_tag_1_banner.txt )
    if [[ -f ../nnpdf31_proc/"${dataset}"/metadata.txt ]]; then
        eval "$(awk -F= "BEGIN { printf \"pineappl write ${grid} ${grid}.tmp \" }
                              { printf \"--set-key-value %s '%s' \", \$1, \$2 }
                        END   { printf \"--set-key-file runcard ${runcard[0]}\\n\" }" \
            ../nnpdf31_proc/"${dataset}"/metadata.txt)"
    else
        "${pineappl}" write "${grid}" "${grid}".tmp --set-key-file runcard "${runcard[0]}"
    fi
    mv "${grid}".tmp "${grid}"

    # find out which PDF set was used to generate the predictions
    pdfstring=$(grep "set lhaid" "${launch_file}" | sed 's/set lhaid \([0-9]\+\)/\1/')

    # (re-)produce predictions
    "${pineappl}" --silence-lhapdf uncert "${grid}" "${pdfstring}" --scale-abs=9 --integrated \
        > pineappl.convolute
    "${pineappl}" --silence-lhapdf orders "${grid}" "${pdfstring}" --absolute > pineappl.orders
    "${pineappl}" --silence-lhapdf uncert --pdf "${grid}" "${pdfstring}" > pineappl.pdf_uncertainty

    # extract the numerical results from mg5_aMC
    sed '/^  [+-]/!d' "${dataset}"/Events/run_01*/MADatNLO.HwU > results.mg5_aMC

    # extract the integrated results from the PineAPPL grid
    tail -n +4 pineappl.convolute | \
        awk '{ print $4, $5, $6, $7, $8, $9, $10, $11, $12, $13 }' > results.grid

    # compare the results from the grid and from mg5_aMC
    paste -d ' ' results.grid results.mg5_aMC | awk \
        'function abs(x) { return x < 0.0 ? -x : x; }
         function max(x1,x2,x3,x4,x5,x6,x7,x8,x9) {
             result=x1;
             if (x2 > result) { result = x2; }
             if (x3 > result) { result = x3; }
             if (x4 > result) { result = x4; }
             if (x5 > result) { result = x5; }
             if (x6 > result) { result = x6; }
             if (x7 > result) { result = x7; }
             if (x8 > result) { result = x8; }
             if (x9 > result) { result = x9; }
             return result;
         }
         function min(x1,x2,x3,x4,x5,x6,x7,x8,x9) {
             result=x1;
             if (x2 < result) { result = x2; }
             if (x3 < result) { result = x3; }
             if (x4 < result) { result = x4; }
             if (x5 < result) { result = x5; }
             if (x6 < result) { result = x6; }
             if (x7 < result) { result = x7; }
             if (x8 < result) { result = x8; }
             if (x9 < result) { result = x9; }
             return result;
         }
         BEGIN { print "----------------------------------------------------------------------"
                 print "   PineAPPL         MC        sigma      central         min      max "
                 print "                              1/100   sigma   1/1000   1/1000   1/1000"
                 print "----------------------------------------------------------------------" }
         { printf "% e % e %7.3f %7.3f %8.4f %8.4f %8.4f\n",
                  $1,
                  $13,
                  $13 != 0.0 ? $14/$13*100 : 0.0,
                  $14 != 0.0 ? abs($1-$13)/$14 : 0.0,
                  $13 != 0.0 ? abs($1-$13)/$13*1000 : 0.0,
                  $16 != 0.0 ? abs(min($2, $3, $4, $5, $6, $7, $8, $9, $10)-$16)/$16*1000 : 0.0,
                  $17 != 0.0 ? abs(max($2, $3, $4, $5, $6, $7, $8, $9, $10)-$17)/$17*1000 : 0.0 }' | tee results.log

    rm results.mg5_aMC results.grid

    runcard_gitversion=$(git describe --long --tags --dirty --always)

    bzr=$(which bzr 2> /dev/null || which brz 2> /dev/null || true)

    if [[ -x "${bzr}" ]] && "${bzr}" info "$(dirname "${mg5amc}")"/.. &>/dev/null; then
        pushd . > /dev/null
        cd "$(dirname "${mg5amc}")"/..

        mg5amc_revno=$("${bzr}" revno)
        mg5amc_repo=$("${bzr}" info | grep 'parent branch' | sed 's/[[:space:]]*parent branch:[[:space:]]*//')

        popd > /dev/null
    else
        echo "warning: couldn't extract mg5_aMC@NLO repository information"

        mg5amc_revno=""
        mg5amc_repo=""
    fi

    "${pineappl}" write "${grid}" "${grid}".tmp \
        --set-key-file results results.log \
        --set-key-value runcard_gitversion "${runcard_gitversion}" \
        --set-key-value mg5amc_revno "${mg5amc_revno}" \
        --set-key-value mg5amc_repo "${mg5amc_repo}" \
        --set-key-value lumi_id_types pdg_mc_ids
    mv "${grid}".tmp "${grid}"

    # if there is anything to do after the run, do it!
    if [[ -x ../nnpdf31_proc/"${dataset}"/postrun.sh ]]; then
        cp ../nnpdf31_proc/"${dataset}"/postrun.sh .
        GRID=$grid ./postrun.sh
    fi
}

check_args_and_cd_output "$@"

if [[ -d $1 ]]; then
    if yesno "Shall I regenerate the grid in ``$1``?"; then
        cd "$1"
        dataset=${1%-[0-9]*}
        launch_file=launch.txt
        output=$1
    fi
else
    # TODO: fix location of `time.log`

    # record the time and write it to `time.log`
    { { time { output_and_launch 2>&3; } } 2>time.log; } 3>&2
fi

merge

if [[ -e time.log ]]; then
    cat time.log
fi

echo "Output stored in ${output}"
