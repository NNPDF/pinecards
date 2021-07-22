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
    "${pip}" install --prefix "${prefix}" "${prefix}"/pineappl2/pineappl_py/target/wheels/pineappl_py*.whl
)}

install_yadism() {(
    mkdir -p "${prefix}"

    pip=$(which pip 2> /dev/null || true)

    if [[ -x ${pip} ]]; then
        pyver=$(python --version | cut -d' ' -f 2 | cut -d. -f1,2)
        export PATH="${prefix}"/bin:${PATH}
        export PYTHONPATH="${prefix}"/lib/python${pyver}/site-packages
        "${pip}" install --prefix "${prefix}" yadism
    fi
)}

# if we've installed dependencies set the correct paths
if [[ -d ${prefix} ]]; then
    pyver=$(python --version | cut -d' ' -f 2 | cut -d. -f1,2)
    export PYTHONPATH="${prefix}"/lib/python${pyver}/site-packages:${PYTHONPATH:-}
    export PATH=${prefix}/mg5amc/bin:${prefix}/bin:${PATH:-}
    export LD_LIBRARY_PATH=${prefix}/lib:${LD_LIBRARY_PATH:-}
    export PKG_CONFIG_PATH=${prefix}/lib/pkgconfig:${PKG_CONFIG_PATH:-}
fi

install_pineappl=
install_yadism=

if [[ ! -d "${prefix}"/pineappl2 ]]; then
    install_pineappl=yes
    echo "PineAPPL (pyo3) wasn't found"
fi

if pip show yadism 2>&1 | grep 'Package(s) not found' > /dev/null; then
    install_yadism=yes
    echo "yadism wasn't found"
fi

if [[ -n ${install_pineappl}${install_yadism} ]]; then
    if yesno "Do you want to install the missing dependencies (into \`.prefix\`)?"; then
        if [[ -n ${install_pineappl} ]]; then
            install_pineappl
        fi
        if [[ -n ${install_yadism} ]]; then
            install_yadism
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

python run_dis.py "$@"
