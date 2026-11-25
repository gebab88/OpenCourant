#!/usr/bin/env bash
# Build the GNU double-precision SMP (OpenMP) solver natively on macOS
# (Apple Silicon) with GCC from MacPorts.
#
# Requirements (MacPorts):
#   sudo port install gcc15 cmake OpenBLAS +lapack metis zlib
#
# No prebuilt hm_reader exists for macOS, so the in-tree open_reader
# (reader/) is built and linked. It lacks some newer reader APIs:
#   - /PART Irigid RBODY conversion is not supported
#   - /ALE/STRUCTURED_MESH is not supported
#
# Usage: bash build_mac.sh [positive number of build jobs]
# Override the compilers with OC_FC / OC_CC / OC_CXX and the MacPorts
# prefix with MACPORTS_PREFIX.
set -euo pipefail

root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
jobs=${1:-$(sysctl -n hw.ncpu)}
if ! [[ "$jobs" =~ ^[1-9][0-9]*$ ]]; then
    printf 'Usage: bash build_mac.sh [positive number of build jobs]\n' >&2
    exit 1
fi

if [ "$(uname -s)" != "Darwin" ]; then
    printf 'build_mac.sh is for macOS; use build_linux.sh on Linux.\n' >&2
    exit 1
fi

prefix=${MACPORTS_PREFIX:-/opt/local}
fc=${OC_FC:-$prefix/bin/gfortran-mp-15}
cc=${OC_CC:-$prefix/bin/gcc-mp-15}
cxx=${OC_CXX:-$prefix/bin/g++-mp-15}

missing=0
for tool in "$fc" "$cc" "$cxx"; do
    if ! command -v "$tool" > /dev/null 2>&1; then
        printf 'Compiler not found: %s\n' "$tool" >&2
        missing=1
    fi
done
for file in include/metis.h lib/libmetis.dylib lib/libopenblas.dylib include/zlib.h lib/libz.dylib; do
    if [ ! -e "$prefix/$file" ]; then
        printf 'Library file not found: %s/%s\n' "$prefix" "$file" >&2
        missing=1
    fi
done
if [ "$missing" = 1 ]; then
    printf '\nInstall the requirements with:\n  sudo port install gcc15 cmake OpenBLAS +lapack metis zlib\n' >&2
    exit 1
fi

python3 "$root/Compiling_tools/script/load_extlib.py"

# open_reader -> exec/libopen_reader_darwin_gf.dylib
cmake -S "$root/reader" -B "$root/reader/cbuild_darwin_gf" \
    -Darch=darwin_gf -Dcom=0 \
    -DCMAKE_C_COMPILER="$cc" -DCMAKE_CXX_COMPILER="$cxx"
cmake --build "$root/reader/cbuild_darwin_gf" --parallel "$jobs"

# Starter -> exec/starter_darwin_gf
cmake -S "$root/starter" -B "$root/starter/cbuild_starter_darwin_gf" \
    -Darch=darwin_gf -Dprecision=dp -Ddebug=0 \
    -DEXEC_NAME=starter_darwin_gf -DUSE_OPEN_READER=1 -Dmacports_prefix="$prefix" \
    -DHM_READER_LEGACY_INCLUDE_API=ON -DHM_READER_LEGACY_PART_API=ON \
    -DHM_READER_NO_RBODIES_FROM_RIGID_PARTS=ON -DHM_READER_NO_STRUCTURED_ALE=ON \
    -DCMAKE_Fortran_COMPILER="$fc" -DCMAKE_C_COMPILER="$cc" -DCMAKE_CXX_COMPILER="$cxx"
cmake --build "$root/starter/cbuild_starter_darwin_gf" --parallel "$jobs"

# Engine -> exec/engine_darwin_gf
cmake -S "$root/engine" -B "$root/engine/cbuild_engine_darwin_gf" \
    -Darch=darwin_gf -Dprecision=dp -Ddebug=0 -DMPI=smp \
    -DEXEC_NAME=engine_darwin_gf \
    -DCMAKE_Fortran_COMPILER="$fc" -DCMAKE_C_COMPILER="$cc" -DCMAKE_CXX_COMPILER="$cxx"
cmake --build "$root/engine/cbuild_engine_darwin_gf" --parallel "$jobs"

printf '\nExecutables are in %s/exec\n' "$root"
printf 'Before running:\n'
printf '  export RAD_CFG_PATH=%s/hm_cfg_files\n' "$root"
printf '  export OMP_STACKSIZE=400m\n'
printf '  export OMP_NUM_THREADS=<threads>\n'
printf 'Reader limitations: /PART Irigid RBODY conversion and /ALE/STRUCTURED_MESH are not supported.\n'
