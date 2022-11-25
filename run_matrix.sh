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

mkdir -p "${cwd}"/{input,log,result}
cp "${card_dir}"/{distribution,model,parameter}.dat "${cwd}"/input/
cp "${matrix_dir}"/run/${process}_MATRIX/input/default.input.MATRIX/runtime.dat "${cwd}"/input/

cd "${cwd}"
# we need an absoute path here
cwd=$(pwd)

cd ${matrix_dir}/run/${process}_MATRIX

run_dir=$(mktemp -du run_XXXXXXXXXX)

# run MATRIX
bin/run_process --run_mode run --input_dir ${cwd}/input ${run_dir}

# copy results
mv "${run_dir}" "${cwd}"/
mv result/${run_dir} "${cwd}"/result/
mv log/${run_dir} "${cwd}"/log/
mv log/${run_dir}.log "${cwd}"/log/

# we already have the input files
rm -r input/${run_dir}
