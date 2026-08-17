#!/bin/sh

# Move file for use with mf, read more at https://github.com/greeenlaser/personal-stash/tree/main/mf

set -e

#
# References
#

case "$1" in
    --linux)
        OUT_NAME=ElypsoEngine-Linux

        LIB_NAME=libelypsoengine
        LIB_EXT=a
        LIB_ORIGIN=build/release-linux
        ;;
    --windows-gnu)
        OUT_NAME=ElypsoEngine-Windows-Gnu
        LIB_NAME=elypsoengine-gnu
        LIB_EXT=lib
        LIB_ORIGIN=build/release-windows-gnu
        ;;
    --windows)
        OUT_NAME=ElypsoEngine-Windows

        LIB_NAME=elypsoengine
        LIB_EXT=lib
        LIB_ORIGIN=build/release-windows
        ;;
    *)
        echo "Error: Argument must be --linux, --windows-gnu or --windows" >&2
        exit 1
        ;;
esac

OUT_VER=0-0-1
OUT_DIR=out/${OUT_NAME}-${OUT_VER}

README=README.md
LICENSE=LICENSE.md
INCLUDE=include
DOCS=docs

DIR_ES=../../external-shared
IN_KH=${DIR_ES}/KalaHeaders
OUT_KH_NAME=kalaheaders

# KalaWindow

IN_KW=${DIR_ES}/KalaWindow
OUT_KW_NAME=kalawindow

# KalaGraphics

IN_KG=${DIR_ES}/KalaGraphics
OUT_KG_NAME=kalagraphics

# KalaPhysics

IN_KP=${DIR_ES}/KalaPhysics
OUT_KP_NAME=kalaphysics

# KalaServer

IN_KS=${DIR_ES}/KalaServer
OUT_KS_NAME=kalaserver

# KalaAudio

IN_KA=${DIR_ES}/KalaAudio
OUT_KA_NAME=kalaaudio

# KalaLua

IN_KL=${DIR_ES}/KalaLua
OUT_KL_NAME=kalalua

# miniaudio

IN_MA=${DIR_ES}/miniaudio
OUT_MA_NAME=miniaudio

# lua

IN_LUA=${DIR_ES}/lua
OUT_LUA_NAME=lua

#
# Core stuff
#

# Always a fresh start
mkdir -p "out"
rm -rf "${OUT_DIR}"
mkdir "${OUT_DIR}"
mkdir "${OUT_DIR}/${OUT_KH_NAME}"

# The base files
mf --f "../${README}" --t "${OUT_DIR}/${README}"
mf --f "../${LICENSE}" --t "${OUT_DIR}/${LICENSE}"
mf --f "${INCLUDE}" --t "${OUT_DIR}"
mf --f "../${DOCS}" --t "${OUT_DIR}"

# The binary
if [ ! -f "${LIB_ORIGIN}/${LIB_NAME}.${LIB_EXT}" ]; then
    printf 'Error: Binary %s not found\n' "${LIB_ORIGIN}/${LIB_NAME}.${LIB_EXT}" >&2
    exit 1
fi

mf --f "${LIB_ORIGIN}/${LIB_NAME}.${LIB_EXT}" --t "${OUT_DIR}/${LIB_NAME}.${LIB_EXT}"

# KalaHeaders
mf --f "${IN_KH}/${README}" --t "${OUT_DIR}/${OUT_KH_NAME}/${README}"
mf --f "${IN_KH}/${LICENSE}" --t "${OUT_DIR}/${OUT_KH_NAME}/${LICENSE}"

mf --f "${IN_KH}/include" --t "${OUT_DIR}/${OUT_KH_NAME}"

# KalaWindow
mkdir "${OUT_DIR}/${OUT_KW_NAME}"

mf --f "${IN_KW}/${README}" --t "${OUT_DIR}/${OUT_KW_NAME}/${README}"
mf --f "${IN_KW}/${LICENSE}" --t "${OUT_DIR}/${OUT_KW_NAME}/${LICENSE}"

mf --f "${IN_KW}/include" --t "${OUT_DIR}/${OUT_KW_NAME}"

# KalaWindow binary
case "$1" in
    --linux)
        mf --f "${IN_KW}/release/libkalawindow.a" --t "${OUT_DIR}/${OUT_KW_NAME}/libkalawindow.a"
        ;;
    --windows-gnu)
        mf --f "${IN_KW}/release/kalawindow-gnu.lib" --t "${OUT_DIR}/${OUT_KW_NAME}/kalawindow-gnu.lib"
        ;;
    --windows)
        mf --f "${IN_KW}/release/kalawindow.lib" --t "${OUT_DIR}/${OUT_KW_NAME}/kalawindow.lib"
        ;;
esac

# KalaGraphics
mkdir "${OUT_DIR}/${OUT_KG_NAME}"

mf --f "${IN_KG}/${README}" --t "${OUT_DIR}/${OUT_KG_NAME}/${README}"
mf --f "${IN_KG}/${LICENSE}" --t "${OUT_DIR}/${OUT_KG_NAME}/${LICENSE}"

mf --f "${IN_KG}/include" --t "${OUT_DIR}/${OUT_KG_NAME}"

# KalaGraphics binary
case "$1" in
    --linux)
        mf --f "${IN_KG}/release/libkalagraphics.a" --t "${OUT_DIR}/${OUT_KG_NAME}/libkalagraphics.a"
        ;;
    --windows-gnu)
        mf --f "${IN_KG}/release/kalagraphics-gnu.lib" --t "${OUT_DIR}/${OUT_KG_NAME}/kalagraphics-gnu.lib"
        ;;
    --windows)
        mf --f "${IN_KG}/release/kalagraphics.lib" --t "${OUT_DIR}/${OUT_KG_NAME}/kalagraphics.lib"
        ;;
esac

# KalaPhysics
mkdir "${OUT_DIR}/${OUT_KP_NAME}"

mf --f "${IN_KP}/${README}" --t "${OUT_DIR}/${OUT_KP_NAME}/${README}"
mf --f "${IN_KP}/${LICENSE}" --t "${OUT_DIR}/${OUT_KP_NAME}/${LICENSE}"

mf --f "${IN_KP}/include" --t "${OUT_DIR}/${OUT_KP_NAME}"

# KalaPhysics binary
case "$1" in
    --linux)
        mf --f "${IN_KP}/release/libkalaphysics.a" --t "${OUT_DIR}/${OUT_KP_NAME}/libkalaphysics.a"
        ;;
    --windows-gnu)
        mf --f "${IN_KP}/release/kalaphysics-gnu.lib" --t "${OUT_DIR}/${OUT_KP_NAME}/kalaphysics-gnu.lib"
        ;;
    --windows)
        mf --f "${IN_KP}/release/kalaphysics.lib" --t "${OUT_DIR}/${OUT_KP_NAME}/kalaphysics.lib"
        ;;
esac

# KalaServer
mkdir "${OUT_DIR}/${OUT_KS_NAME}"

mf --f "${IN_KS}/${README}" --t "${OUT_DIR}/${OUT_KS_NAME}/${README}"
mf --f "${IN_KS}/${LICENSE}" --t "${OUT_DIR}/${OUT_KS_NAME}/${LICENSE}"

mf --f "${IN_KS}/include" --t "${OUT_DIR}/${OUT_KS_NAME}"

# KalaServer binary
case "$1" in
    --linux)
        mf --f "${IN_KS}/release/libkalaserver.a" --t "${OUT_DIR}/${OUT_KS_NAME}/libkalaserver.a"
        ;;
    --windows-gnu)
        mf --f "${IN_KS}/release/kalaserver-gnu.lib" --t "${OUT_DIR}/${OUT_KS_NAME}/kalaserver-gnu.lib"
        ;;
    --windows)
        mf --f "${IN_KS}/release/kalaserver.lib" --t "${OUT_DIR}/${OUT_KS_NAME}/kalaserver.lib"
        ;;
esac

# KalaAudio
mkdir "${OUT_DIR}/${OUT_KA_NAME}"

mf --f "${IN_KA}/${README}" --t "${OUT_DIR}/${OUT_KA_NAME}/${README}"
mf --f "${IN_KA}/${LICENSE}" --t "${OUT_DIR}/${OUT_KA_NAME}/${LICENSE}"

mf --f "${IN_KA}/include" --t "${OUT_DIR}/${OUT_KA_NAME}"

# KalaAudio binary
case "$1" in
    --linux)
        mf --f "${IN_KA}/release/libkalaaudio.a" --t "${OUT_DIR}/${OUT_KA_NAME}/libkalaaudio.a"
        ;;
    --windows-gnu)
        mf --f "${IN_KA}/release/kalaaudio-gnu.lib" --t "${OUT_DIR}/${OUT_KA_NAME}/kalaaudio-gnu.lib"
        ;;
    --windows)
        mf --f "${IN_KA}/release/kalaaudio.lib" --t "${OUT_DIR}/${OUT_KA_NAME}/kalaaudio.lib"
        ;;
esac

# KalaLua
mkdir "${OUT_DIR}/${OUT_KL_NAME}"

mf --f "${IN_KL}/${README}" --t "${OUT_DIR}/${OUT_KL_NAME}/${README}"
mf --f "${IN_KL}/${LICENSE}" --t "${OUT_DIR}/${OUT_KL_NAME}/${LICENSE}"

mf --f "${IN_KL}/include" --t "${OUT_DIR}/${OUT_KL_NAME}"

# KalaLua binary
case "$1" in
    --linux)
        mf --f "${IN_KL}/release/libkalalua.a" --t "${OUT_DIR}/${OUT_KL_NAME}/libkalalua.a"
        ;;
    --windows-gnu)
        mf --f "${IN_KL}/release/kalalua-gnu.lib" --t "${OUT_DIR}/${OUT_KL_NAME}/kalalua-gnu.lib"
        ;;
    --windows)
        mf --f "${IN_KL}/release/kalalua.lib" --t "${OUT_DIR}/${OUT_KL_NAME}/kalalua.lib"
        ;;
esac

# Miniaudio
mkdir "${OUT_DIR}/${OUT_MA_NAME}"

mf --f "${IN_MA}/${README}" --t "${OUT_DIR}/${OUT_MA_NAME}/${README}"
mf --f "${IN_MA}/${LICENSE}" --t "${OUT_DIR}/${OUT_MA_NAME}/${LICENSE}"

mf --f "${IN_MA}/include" --t "${OUT_DIR}/${OUT_MA_NAME}"

# Miniaudio binary
case "$1" in
    --linux)
        mf --f "${IN_MA}/release/libminiaudio.a" --t "${OUT_DIR}/${OUT_MA_NAME}/libminiaudio.a"
        ;;
    --windows-gnu)
        mf --f "${IN_MA}/release/miniaudio-gnu.lib" --t "${OUT_DIR}/${OUT_MA_NAME}/miniaudio-gnu.lib"
        ;;
    --windows)
        mf --f "${IN_MA}/release/miniaudio.lib" --t "${OUT_DIR}/${OUT_MA_NAME}/miniaudio.lib"
        ;;
esac

# Lua
mkdir "${OUT_DIR}/${OUT_LUA_NAME}"

mf --f "${IN_LUA}/${README}" --t "${OUT_DIR}/${OUT_LUA_NAME}/${README}"
mf --f "${IN_LUA}/${LICENSE}" --t "${OUT_DIR}/${OUT_LUA_NAME}/${LICENSE}"

mf --f "${IN_LUA}/include" --t "${OUT_DIR}/${OUT_LUA_NAME}"

# Lua binary
case "$1" in
    --linux)
        mf --f "${IN_LUA}/release/liblua.a" --t "${OUT_DIR}/${OUT_LUA_NAME}/liblua.a"
        ;;
    --windows-gnu)
        mf --f "${IN_LUA}/release/lua-gnu.lib" --t "${OUT_DIR}/${OUT_LUA_NAME}/lua-gnu.lib"
        ;;
    --windows)
        mf --f "${IN_LUA}/release/lua.lib" --t "${OUT_DIR}/${OUT_LUA_NAME}/lua.lib"
        ;;
esac
