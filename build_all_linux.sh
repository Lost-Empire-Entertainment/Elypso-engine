#!/bin/bash

# Set root folder
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Set shell script paths
ENGINE_RELEASE_SH="${ROOT_DIR}/Engine/build_linux_release.sh"
ENGINE_DEBUG_SH="${ROOT_DIR}/Engine/build_linux_debug.sh"
ENGINE_LIB_RELEASE_SH="${ROOT_DIR}/Engine library/build_linux_release.sh"
ENGINE_LIB_DEBUG_SH="${ROOT_DIR}/Engine library/build_linux_debug.sh"
COPY_SH="${ROOT_DIR}/_external_shared/copy_linux.sh"

# Confirm if shell script paths are valid
if [[ ! -f "$COPY_SH" ]]; then
    echo "[ERROR] Copy shell script does not exist: $COPY_SH"
    read -r -p "Press enter to exit..."
    exit 1
fi
if [[ ! -f "$RELEASE_SH" ]]; then
    echo "[ERROR] Release shell script does not exist: $RELEASE_SH"
    read -r -p "Press enter to exit..."
    exit 1
fi
if [[ ! -f "$DEBUG_SH" ]]; then
    echo "[ERROR] Debug shell script does not exist: $DEBUG_SH"
    read -r -p "Press enter to exit..."
    exit 1
fi

echo "====================================="
echo "[INFO] Copying binaries..."
echo "====================================="
echo ""

# Copy external shared
if ! bash "${COPY_SH}"; then
    echo "[ERROR] Failed to copy binaries."
    read -r -p "Press enter to exit..."
    exit 1
fi

echo "====================================="
echo "[INFO] Building Elypso engine in Release mode..."
echo "====================================="
echo ""

if ! bash "${ENGINE_RELEASE_SH}"; then
    echo "[ERROR] Engine Release build failed!"
    read -r -p "Press enter to exit..."
    exit 1
fi

echo "====================================="
echo "[INFO] Building Elypso engine in Debug mode..."
echo "====================================="
echo ""

if ! bash "${ENGINE_DEBUG_SH}"; then
    echo "[ERROR] Engine Debug build failed!"
    read -r -p "Press enter to exit..."
    exit 1
fi

echo "====================================="
echo "[INFO] Building Elypso engine library in Release mode..."
echo "====================================="
echo ""

if ! bash "${ENGINE_LIB_RELEASE_SH}"; then
    echo "[ERROR] Engine Library Release build failed!"
    read -r -p "Press enter to exit..."
    exit 1
fi

echo "====================================="
echo "[INFO] Building Elypso engine library in Debug mode..."
echo "====================================="
echo ""

if ! bash "${ENGINE_LIB_DEBUG_SH}"; then
    echo "[ERROR] Engine Library Debug build failed!"
    read -r -p "Press enter to exit..."
    exit 1
fi

echo ""
echo "====================================="
echo "[SUCCESS] Finished building and installing Elypso engine and Elypso engine library!"
echo "====================================="
echo ""

read -p "Press enter to continue..."
exit 0
