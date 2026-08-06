#!/bin/sh

# Move file for use with mf, read more at https://github.com/greeenlaser/personal-stash/tree/main/mf

set -e

#
# References
#

case "$1" in
    --linux)
        OUT_NAME=GameTemplate-Linux

        EXE_NAME=game-template
        EXE_EXT=
        EXE_ORIGIN=build/release-linux
        ;;
    --windows-gnu)
        OUT_NAME=GameTemplate-Win-on-Linux

        EXE_NAME=game-template
        EXE_EXT=.exe
        EXE_ORIGIN=build/release-windows-gnu
        ;;
    --windows)
        OUT_NAME=GameTemplate-Windows

        EXE_NAME=game-template
        EXE_EXT=.exe
        EXE_ORIGIN=build/release-windows
        ;;
    *)
        echo "Error: Argument must be --linux, --windows-gnu or --windows" >&2
        exit 1
        ;;
esac

OUT_VER=0-0-1
OUT_DIR=out/${OUT_NAME}-${OUT_VER}

README=../README.md
LICENSE=../LICENSE.md
DOCS=../docs

#
# Core stuff
#

# Always a fresh start
mkdir -p "out"
rm -rf "${OUT_DIR}"
mkdir "${OUT_DIR}"

# The base files
mf --f "${README}" --t "${OUT_DIR}/${README}"
mf --f "${LICENSE}" --t "${OUT_DIR}/${LICENSE}"
mf --f "${DOCS}" --t "${OUT_DIR}"

# The binary
if [ ! -f "${EXE_ORIGIN}/${EXE_NAME}${EXE_EXT}" ]; then
    printf 'Error: Binary %s not found\n' "${EXE_ORIGIN}/${EXE_NAME}${EXE_EXT}" >&2
    exit 1
fi

mf --f "${EXE_ORIGIN}/${EXE_NAME}${EXE_EXT}" --t "${OUT_DIR}/${EXE_NAME}${EXE_EXT}"

# Copy windows-gnu dlls
case "$1" in
    --windows-gnu)
        mf --f "${EXE_ORIGIN}/libgcc_s_seh-1.dll" --t "${OUT_DIR}/libgcc_s_seh-1.dll"
        mf --f "${EXE_ORIGIN}/libstdc++-6.dll" --t "${OUT_DIR}/libstdc++-6.dll"
        mf --f "${EXE_ORIGIN}/libwinpthread-1.dll" --t "${OUT_DIR}/libwinpthread-1.dll"
    ;;
esac
