#!/bin/sh

# Move file for use with mf, read more at https://github.com/greeenlaser/personal-stash/tree/main/mf

set -e

#
# References
#

VERSION=0-0-1
BIN_NAME=elypsoengine

BUILD_DIR=build
TEMP_DIR=${BUILD_DIR}/temp
EXTERNAL_DIR=../external

KMAKE_ORIGIN=project.kmake

KH_DIR=${EXTERNAL_DIR}/kalaheaders
KW_DIR=${EXTERNAL_DIR}/kalawindow
KG_DIR=${EXTERNAL_DIR}/kalagraphics
KP_DIR=${EXTERNAL_DIR}/kalaphysics
KA_DIR=${EXTERNAL_DIR}/kalaaudio
KL_DIR=${EXTERNAL_DIR}/kalalua

if [ ! -d "${EXTERNAL_DIR}" ]; then
    echo "[ERROR] Failed to compile ${BIN_NAME} because '../external' was not found!"
    exit 1
fi

case "$1" in
    --linux)
        BIN_NAME_FRONT=lib
        BIN_NAME_BACK=
        BIN_EXT=.a

        BUILD_RELEASE="--compile ${KMAKE_ORIGIN} release-linux"
        BUILD_DEBUG="--compile ${KMAKE_ORIGIN} debug-linux"

        TEMP_REL_DIR=${TEMP_DIR}/release-linux
        TEMP_DEB_DIR=${TEMP_DIR}/debug-linux

        TARGET_REL_DIR=${BUILD_DIR}/${VERSION}/release-linux
        TARGET_DEB_DIR=${BUILD_DIR}/${VERSION}/debug-linux

        SOURCE_KW_REL_DIR=${KW_DIR}/release-linux
        SOURCE_KW_DEB_DIR=${KW_DIR}/debug-linux

        SOURCE_KG_REL_DIR=${KG_DIR}/release-linux
        SOURCE_KG_DEB_DIR=${KG_DIR}/debug-linux

        SOURCE_KP_REL_DIR=${KP_DIR}/release-linux
        SOURCE_KP_DEB_DIR=${KP_DIR}/debug-linux

        SOURCE_KA_REL_DIR=${KA_DIR}/release-linux
        SOURCE_KA_DEB_DIR=${KA_DIR}/debug-linux

        SOURCE_KL_REL_DIR=${KL_DIR}/release-linux
        SOURCE_KL_DEB_DIR=${KL_DIR}/debug-linux
        ;;
    --windows-gnu)
        BIN_NAME_FRONT=
        BIN_NAME_BACK=-gnu
        BIN_EXT=.lib

        BUILD_RELEASE="--compile ${KMAKE_ORIGIN} release-windows-gnu"
        BUILD_DEBUG="--compile ${KMAKE_ORIGIN} debug-windows-gnu"

        TEMP_REL_DIR=${TEMP_DIR}/release-windows-gnu
        TEMP_DEB_DIR=${TEMP_DIR}/debug-windows-gnu

        TARGET_REL_DIR=${BUILD_DIR}/${VERSION}/release-windows-gnu
        TARGET_DEB_DIR=${BUILD_DIR}/${VERSION}/debug-windows-gnu

        SOURCE_LUA_REL_DIR=${LUA_DIR}/release-windows-gnu
        SOURCE_LUA_DEB_DIR=${LUA_DIR}/debug-windows-gnu

        SOURCE_KW_REL_DIR=${KW_DIR}/release-windows-gnu
        SOURCE_KW_DEB_DIR=${KW_DIR}/debug-windows-gnu

        SOURCE_KG_REL_DIR=${KG_DIR}/release-windows-gnu
        SOURCE_KG_DEB_DIR=${KG_DIR}/debug-windows-gnu

        SOURCE_KP_REL_DIR=${KP_DIR}/release-windows-gnu
        SOURCE_KP_DEB_DIR=${KP_DIR}/debug-windows-gnu

        SOURCE_KA_REL_DIR=${KA_DIR}/release-windows-gnu
        SOURCE_KA_DEB_DIR=${KA_DIR}/debug-windows-gnu

        SOURCE_KL_REL_DIR=${KL_DIR}/release-windows-gnu
        SOURCE_KL_DEB_DIR=${KL_DIR}/debug-windows-gnu
        ;;
    --windows)
        BIN_NAME_FRONT=
        BIN_NAME_BACK=
        BIN_EXT=.lib

        BUILD_RELEASE="--compile ${KMAKE_ORIGIN} release-windows"
        BUILD_DEBUG="--compile ${KMAKE_ORIGIN} debug-windows"

        TEMP_REL_DIR=${TEMP_DIR}/release-windows
        TEMP_DEB_DIR=${TEMP_DIR}/debug-windows

        TARGET_REL_DIR=${BUILD_DIR}/${VERSION}/release-windows
        TARGET_DEB_DIR=${BUILD_DIR}/${VERSION}/debug-windows

        SOURCE_LUA_REL_DIR=${LUA_DIR}/release-windows
        SOURCE_LUA_DEB_DIR=${LUA_DIR}/debug-windows

        SOURCE_KW_REL_DIR=${KW_DIR}/release-windows
        SOURCE_KW_DEB_DIR=${KW_DIR}/debug-windows

        SOURCE_KG_REL_DIR=${KG_DIR}/release-windows
        SOURCE_KG_DEB_DIR=${KG_DIR}/debug-windows

        SOURCE_KP_REL_DIR=${KP_DIR}/release-windows
        SOURCE_KP_DEB_DIR=${KP_DIR}/debug-windows

        SOURCE_KA_REL_DIR=${KA_DIR}/release-windows
        SOURCE_KA_DEB_DIR=${KA_DIR}/debug-windows

        SOURCE_KL_REL_DIR=${KL_DIR}/release-windows
        SOURCE_KL_DEB_DIR=${KL_DIR}/debug-windows
        ;;
    *)
        echo "Error: Argument must be --linux, --windows-gnu or --windows" >&2
        exit 1
        ;;
esac

case "$2" in
    --export)
        ;;
    "")
        ;;
    *)
        echo "Error: Second argument must be --export or empty" >&2
        exit 1
        ;;
esac

#
# Compile
#

if [ ! -d "${BUILD_DIR}" ]; then
    mkdir "${BUILD_DIR}"
fi

if [ "$2" = "--export" ]; then
    if [ -d "${TEMP_DIR}" ]; then
        rm -rf "${TEMP_DIR}"
    fi
fi

if [ ! -d "${BUILD_DIR}/${VERSION}" ]; then
    mkdir "${BUILD_DIR}/${VERSION}"
fi

kalamake ${BUILD_RELEASE} || exit

if [ "$2" = "" ]; then
    kalamake ${BUILD_DEBUG} || exit 1
fi

#
# Copy docs and dependencies
#

# Release

BIN_REL=${BIN_NAME_FRONT}${BIN_NAME}${BIN_NAME_BACK}${BIN_EXT}

if [ ! -d "${TARGET_REL_DIR}" ]; then
    mkdir "${TARGET_REL_DIR}"
fi

mf --o --f "${TEMP_REL_DIR}/${BIN_REL}" --t "${TARGET_REL_DIR}/${BIN_REL}"
mf --o --f "include" --t "${TARGET_REL_DIR}"

mf --o --f "../README.md" --t "${TARGET_REL_DIR}/README.md"
mf --o --f "../LICENSE.md" --t "${TARGET_REL_DIR}/LICENSE.md"
mf --o --f "CHANGES.md" --t "${TARGET_REL_DIR}/CHANGES.md"

mf --o --f "../docs" --t "${TARGET_REL_DIR}"

mf --o --f "${KH_DIR}" --t "${TARGET_REL_DIR}"

if [ ! -d "${TARGET_REL_DIR}/kalawindow" ]; then
    mkdir "${TARGET_REL_DIR}/kalawindow"
    cp -R "${SOURCE_KW_REL_DIR}/." "${TARGET_REL_DIR}/kalawindow/"
fi

if [ ! -d "${TARGET_REL_DIR}/kalagraphics" ]; then
    mkdir "${TARGET_REL_DIR}/kalagraphics"
    cp -R "${SOURCE_KG_REL_DIR}/." "${TARGET_REL_DIR}/kalagraphics/"
fi

if [ ! -d "${TARGET_REL_DIR}/kalaphysics" ]; then
    mkdir "${TARGET_REL_DIR}/kalaphysics"
    cp -R "${SOURCE_KP_REL_DIR}/." "${TARGET_REL_DIR}/kalaphysics/"
fi

if [ ! -d "${TARGET_REL_DIR}/kalaaudio" ]; then
    mkdir "${TARGET_REL_DIR}/kalaaudio"
    cp -R "${SOURCE_KA_REL_DIR}/." "${TARGET_REL_DIR}/kalaaudio/"
fi

if [ ! -d "${TARGET_REL_DIR}/kalalua" ]; then
    mkdir "${TARGET_REL_DIR}/kalalua"
    cp -R "${SOURCE_KL_REL_DIR}/." "${TARGET_REL_DIR}/kalalua/"
fi

# Debug

BIN_DEB=${BIN_NAME_FRONT}${BIN_NAME}${BIN_NAME_BACK}d${BIN_EXT}

if [ "$2" = "--export" ]; then
    if [ -d "${TARGET_DEB_DIR}" ]; then
        rm -rf "${TARGET_DEB_DIR}"
    fi
else
    if [ -d "${TARGET_DEB_DIR}" ]; then
        rm -rf "${TARGET_DEB_DIR}"
    fi
    mkdir "${TARGET_DEB_DIR}"

    mf --o --f "${TEMP_DEB_DIR}/${BIN_DEB}" --t "${TARGET_DEB_DIR}/${BIN_DEB}"
    mf --o --f "include" --t "${TARGET_DEB_DIR}"

    mf --o --f "../README.md" --t "${TARGET_DEB_DIR}/README.md"
    mf --o --f "../LICENSE.md" --t "${TARGET_DEB_DIR}/LICENSE.md"
    mf --o --f "CHANGES.md" --t "${TARGET_DEB_DIR}/CHANGES.md"

    mf --o --f "../docs" --t "${TARGET_REL_DIR}"

    mf --o --f "${KH_DIR}" --t "${TARGET_DEB_DIR}"

    if [ ! -d "${TARGET_DEB_DIR}/kalawindow" ]; then
        mkdir "${TARGET_DEB_DIR}/kalawindow"
        cp -R "${SOURCE_KW_DEB_DIR}/." "${TARGET_DEB_DIR}/kalawindow/"
    fi

    if [ ! -d "${TARGET_DEB_DIR}/kalagraphics" ]; then
        mkdir "${TARGET_DEB_DIR}/kalagraphics"
        cp -R "${SOURCE_KG_DEB_DIR}/." "${TARGET_DEB_DIR}/kalagraphics/"
    fi

    if [ ! -d "${TARGET_DEB_DIR}/kalaphysics" ]; then
        mkdir "${TARGET_DEB_DIR}/kalaphysics"
        cp -R "${SOURCE_KP_DEB_DIR}/." "${TARGET_DEB_DIR}/kalaphysics/"
    fi

    if [ ! -d "${TARGET_DEB_DIR}/kalaaudio" ]; then
        mkdir "${TARGET_DEB_DIR}/kalaaudio"
        cp -R "${SOURCE_KA_DEB_DIR}/." "${TARGET_DEB_DIR}/kalaaudio/"
    fi

    if [ ! -d "${TARGET_DEB_DIR}/kalalua" ]; then
        mkdir "${TARGET_DEB_DIR}/kalalua"
        cp -R "${SOURCE_KL_DEB_DIR}/." "${TARGET_DEB_DIR}/kalalua/"
    fi
fi

#
# Cleanup
#

if [ "$2" = "--export" ]; then
    if [ -d "${TARGET_REL_DIR}/obj" ]; then
        rm -rf "${TARGET_REL_DIR}/obj"
    fi

    if [ -d "${TARGET_DEB_DIR}/obj" ]; then
        rm -rf "${TARGET_DEB_DIR}/obj"
    fi

    if [ -d "${TEMP_DIR}" ]; then
        rm -rf "${TEMP_DIR}"
    fi
fi
