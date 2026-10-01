#!/bin/sh

# Move file for use with mf, read more at https://github.com/greeenlaser/personal-stash/tree/main/mf

set -e

#
# References
#

EXTERNAL_DIR=external

KH_ORIGIN=../kalaheaders
KH_TARGET=${EXTERNAL_DIR}/kalaheaders

KW_ORIGIN=../kalawindow/build/1-7-0
KW_TARGET=${EXTERNAL_DIR}/kalawindow

KG_ORIGIN=../kalagraphics/build/0-0-1
KG_TARGET=${EXTERNAL_DIR}/kalagraphics

KP_ORIGIN=../kalaphysics/build/0-0-1
KP_TARGET=${EXTERNAL_DIR}/kalaphysics

KA_ORIGIN=../kalaaudio/build/1-2-0
KA_TARGET=${EXTERNAL_DIR}/kalaaudio

KL_ORIGIN=../kalalua/build/1-1-0
KL_TARGET=${EXTERNAL_DIR}/kalalua

#
# Copy dependencies
#

# Always a fresh start
rm -rf "${EXTERNAL_DIR}"
mkdir "${EXTERNAL_DIR}"

# KalaHeaders
mkdir "${KH_TARGET}"

mf --f "${KH_ORIGIN}/README.md" --t "${KH_TARGET}/README.md"
mf --f "${KH_ORIGIN}/LICENSE.md" --t "${KH_TARGET}/LICENSE.md"

mf --f "${KH_ORIGIN}/include" --t "${KH_TARGET}"

# KalaWindow
mkdir "${KW_TARGET}"

if [ -d "${KW_ORIGIN}/release-windows" ]; then
    mf --f "${KW_ORIGIN}/release-windows" --t "${KW_TARGET}"
fi
if [ -d "${KW_ORIGIN}/release-windows-gnu" ]; then
    mf --f "${KW_ORIGIN}/release-windows-gnu" --t "${KW_TARGET}"
fi
if [ -d "${KW_ORIGIN}/release-linux" ]; then
    mf --f "${KW_ORIGIN}/release-linux" --t "${KW_TARGET}"
fi

if [ -d "${KW_ORIGIN}/debug-windows" ]; then
    mf --f "${KW_ORIGIN}/debug-windows" --t "${KW_TARGET}"
fi
if [ -d "${KW_ORIGIN}/debug-windows-gnu" ]; then
    mf --f "${KW_ORIGIN}/debug-windows-gnu" --t "${KW_TARGET}"
fi
if [ -d "${KW_ORIGIN}/debug-linux" ]; then
    mf --f "${KW_ORIGIN}/debug-linux" --t "${KW_TARGET}"
fi

# KalaGraphics
mkdir "${KG_TARGET}"

if [ -d "${KG_ORIGIN}/release-windows" ]; then
    mf --f "${KG_ORIGIN}/release-windows" --t "${KG_TARGET}"
fi
if [ -d "${KG_ORIGIN}/release-windows-gnu" ]; then
    mf --f "${KG_ORIGIN}/release-windows-gnu" --t "${KG_TARGET}"
fi
if [ -d "${KG_ORIGIN}/release-linux" ]; then
    mf --f "${KG_ORIGIN}/release-linux" --t "${KG_TARGET}"
fi

if [ -d "${KG_ORIGIN}/debug-windows" ]; then
    mf --f "${KG_ORIGIN}/debug-windows" --t "${KG_TARGET}"
fi
if [ -d "${KG_ORIGIN}/debug-windows-gnu" ]; then
    mf --f "${KG_ORIGIN}/debug-windows-gnu" --t "${KG_TARGET}"
fi
if [ -d "${KG_ORIGIN}/debug-linux" ]; then
    mf --f "${KG_ORIGIN}/debug-linux" --t "${KG_TARGET}"
fi

# KalaPhysics
mkdir "${KP_TARGET}"

if [ -d "${KP_ORIGIN}/release-windows" ]; then
    mf --f "${KP_ORIGIN}/release-windows" --t "${KP_TARGET}"
fi
if [ -d "${KP_ORIGIN}/release-windows-gnu" ]; then
    mf --f "${KP_ORIGIN}/release-windows-gnu" --t "${KP_TARGET}"
fi
if [ -d "${KP_ORIGIN}/release-linux" ]; then
    mf --f "${KP_ORIGIN}/release-linux" --t "${KP_TARGET}"
fi

if [ -d "${KP_ORIGIN}/debug-windows" ]; then
    mf --f "${KP_ORIGIN}/debug-windows" --t "${KP_TARGET}"
fi
if [ -d "${KP_ORIGIN}/debug-windows-gnu" ]; then
    mf --f "${KP_ORIGIN}/debug-windows-gnu" --t "${KP_TARGET}"
fi
if [ -d "${KP_ORIGIN}/debug-linux" ]; then
    mf --f "${KP_ORIGIN}/debug-linux" --t "${KP_TARGET}"
fi

# KalaAudio
mkdir "${KA_TARGET}"

if [ -d "${KA_ORIGIN}/release-windows" ]; then
    mf --f "${KA_ORIGIN}/release-windows" --t "${KA_TARGET}"
fi
if [ -d "${KA_ORIGIN}/release-windows-gnu" ]; then
    mf --f "${KA_ORIGIN}/release-windows-gnu" --t "${KA_TARGET}"
fi
if [ -d "${KA_ORIGIN}/release-linux" ]; then
    mf --f "${KA_ORIGIN}/release-linux" --t "${KA_TARGET}"
fi

if [ -d "${KA_ORIGIN}/debug-windows" ]; then
    mf --f "${KA_ORIGIN}/debug-windows" --t "${KA_TARGET}"
fi
if [ -d "${KA_ORIGIN}/debug-windows-gnu" ]; then
    mf --f "${KA_ORIGIN}/debug-windows-gnu" --t "${KA_TARGET}"
fi
if [ -d "${KA_ORIGIN}/debug-linux" ]; then
    mf --f "${KA_ORIGIN}/debug-linux" --t "${KA_TARGET}"
fi

# KalaLua
mkdir "${KL_TARGET}"

if [ -d "${KL_ORIGIN}/release-windows" ]; then
    mf --f "${KL_ORIGIN}/release-windows" --t "${KL_TARGET}"
fi
if [ -d "${KL_ORIGIN}/release-windows-gnu" ]; then
    mf --f "${KL_ORIGIN}/release-windows-gnu" --t "${KL_TARGET}"
fi
if [ -d "${KL_ORIGIN}/release-linux" ]; then
    mf --f "${KL_ORIGIN}/release-linux" --t "${KL_TARGET}"
fi

if [ -d "${KL_ORIGIN}/debug-windows" ]; then
    mf --f "${KL_ORIGIN}/debug-windows" --t "${KL_TARGET}"
fi
if [ -d "${KL_ORIGIN}/debug-windows-gnu" ]; then
    mf --f "${KL_ORIGIN}/debug-windows-gnu" --t "${KL_TARGET}"
fi
if [ -d "${KL_ORIGIN}/debug-linux" ]; then
    mf --f "${KL_ORIGIN}/debug-linux" --t "${KL_TARGET}"
fi
