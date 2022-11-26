#!/bin/bash -x

set -euo pipefail

if [[ $# -lt 2 ]]; then
    echo >&2 "usage: ./run_matrix MATRIXDIR DATASET"
    exit 1
fi

matrix_dir=$1
card_dir=pinecards/$2
cwd=$2-$(date +%Y%m%d%H%M%S)

if [[ ! -d ${card_dir} ]]; then
    echo >&2 "card directory doesn't exist"
    exit 1
fi

card_dir=$(cd "${card_dir}" && pwd)

if [[ ! -x ${matrix_dir}/matrix ]]; then
    echo >&2 "matrix binary not found"
    exit 1
fi

if [[ ! -f ${matrix_dir}/config/MATRIX_configuration ]]; then
    # copy the default configuration file if there isn't any yet
    cp ${matrix_dir}/config/MATRIX_configuration_default ${matrix_dir}/config/MATRIX_configuration
fi

# we need to add the path to PineAPPL
sed -i "s:#path_to_pineappl .*:path_to_pineappl = $(pkg-config --variable=prefix pineappl_capi):" \
    ${matrix_dir}/config/MATRIX_configuration

process=$(cat "${card_dir}"/process)

# if the process hasn't been compiled, we have to do that before we can run the process
if [[ ! -x ${matrix_dir}/bin/${process} ]]; then
    cd ${matrix_dir}
    ./matrix --agree_to_all ${process}
    cd -
fi

mkdir "${cwd}"
cd "${cwd}"
# we need an absoute path here
cwd=$(pwd)

cd ${matrix_dir}/run/${process}_MATRIX

last_run=0

if [[ -d log ]]; then
    cd log
    # log files aren't cleaned, so we should avoid overwriting those. Pick the last
    # log file with an integer in its name
    last_run=$(for i in run_*.log; do
        i=${i#run_}
        i=${i%.log}
        if [[ $i =~ ^[0-9]+$ ]]; then
            echo $i
        fi
    done | sort -n | tail -n1)
    cd -
fi

# increase the integer found by one and pad with zeros
run_dir=run_$(printf "%03d" $(( $((last_run)) + 1)))

mkdir -p input/${run_dir}

# copy input files
cp "${card_dir}"/{distribution,model,parameter}.dat input/${run_dir}/

# run MATRIX
bin/run_process --run_mode run ${run_dir}

# copy results
bin/run_process --run_mode tar_run ${run_dir}

# clean up
bin/run_process --run_mode delete_run ${run_dir}

# copy the tar'ed results
mv ${run_dir}.tar "${cwd}"/
