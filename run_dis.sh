#!/bin/bash

prefix=$(pwd)/.prefix

export LC_ALL=C

install_pineappl() {(
    mkdir -p "${prefix}"

    cargo=$(which cargo 2> /dev/null || true)
    git=$(which git 2> /dev/null)
    pip=$(which pip 2> /dev/null || true)
    maturin=$(which maturin 2> /dev/null || true)

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

    if [[ -d ${prefix}/pineappl2 ]]; then
        "${git}" pull
    else
        "${git}" clone ${repo} -b pyo3 "${prefix}"/pineappl2
    fi

    if [[ ! -x ${maturin} ]]; then
        pyver=$(python --version | cut -d' ' -f 2 | cut -d. -f1,2)
        export PATH="${prefix}"/bin:${PATH}
        export PYTHONPATH="${prefix}"/lib/python${pyver}/site-packages
        "${pip}" install --prefix "${prefix}" maturin
    fi

    pushd . > /dev/null
    cd "${prefix}"/pineappl2/pineappl_py
    maturin build --manylinux off
    popd > /dev/null
    "${pip}" install --prefix "${prefix}" "${prefix}"/pineappl2/target/wheels/pineappl_py*.whl
)}

install_yadism() {
    mkdir -p "${prefix}"

    pip=$(which pip 2> /dev/null || true)


    if [[ -x ${pip} ]]; then
        pyver=$(python --version | cut -d' ' -f 2 | cut -d. -f1,2)
        export PATH="${prefix}"/bin:${PATH}
        export PYTHONPATH="${prefix}"/lib/python${pyver}/site-packages
        "${pip}" install --prefix "${prefix}" yadism
    fi

}

install_pineappl
install_yadism

python run_dis.py "$@"
