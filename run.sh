#!/bin/bash

# exit script at the first sign of an error
set -o errexit

# the following exits if undeclared variables are used
set -o nounset

# check arguments of the script
if (( $# != 1 )); then
    echo "Usage: ./run.sh [nameset]" >&2
    echo "  The following namesets are available:" >&2

    for i in $(find nnpdf31_proc/analyses -name *.f); do
        name=${i##*/}
        nameset=${name%%.f}
        echo "  - ${nameset}" >&2
    done

    exit 2
fi

# name of the experiment
experiment="$1"

# name of the directory where the output is written to
output="$experiment"-$(date +%Y%m%d%H%M%S)

if [[ -d $output ]]; then
    # since we add a date postfix to the name this shouldn't happen
    echo "Error: output directory already exists" >&2
    exit 2
fi

mkdir "${output}"

mg5amc=$(which mg5_aMC)

if [[ ! -x ${mg5amc} ]]; then
    echo "The binary \`mg5_aMC\' wasn't found. Please adjust your PATH variable" >&2
    exit 1
fi

# create a temporary directory and delete it when exiting
tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT

# copy the output file to the temporary directory and replace the variables
mkdir "$tmpdir"/output
output_file="$tmpdir"/output/$experiment.txt
cp nnpdf31_proc/output/$experiment.txt "$output_file"
sed -i "s/@OUTPUT@/$experiment/g" "$output_file"

cd "${output}"

# create output folder
python2 "${mg5amc}" "$output_file"

# copy patches if there are any
if [[ -d ../nnpdf31_proc/patches/$experiment ]]; then
    cp -r ../nnpdf31_proc/patches/$experiment/* "${experiment}"/
fi

# enforce proper analysis
cp ../nnpdf31_proc/analyses/$experiment.f "${experiment}"/FixedOrderAnalysis
sed -i "s/analysis_HwU_template/$experiment/g" "${experiment}"/Cards/FO_analyse_card.dat

# copy the launch file to the temporary directory and replace the variables
mkdir "$tmpdir"/launch
launch_file="$tmpdir"/launch/$experiment.txt
cp ../nnpdf31_proc/launch/$experiment.txt "$launch_file"
sed -i "s/@OUTPUT@/$experiment/g" "$launch_file"

# write a list with variables that should be replaced in the launch file; for the time being we
# create the file here, but in the future it should be read from the theory database
cat > "$tmpdir"/variables.txt <<EOF
MT 172.5
MZ 91.176
YMT 172.5
EOF

# replace the variables with their values
sed -f <(sed -E 's|(.*) (.*)|s/@\1@/\2/|g' "$tmpdir"/variables.txt) -i "$launch_file"

# launch run
python2 "${mg5amc}" "$launch_file"
