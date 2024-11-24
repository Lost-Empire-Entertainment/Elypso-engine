#!/bin/bash

# Define message variables
prexc="[PROGRAM_EXCEPTION]"
prinf="[PROGRAM_INFO]"
prcln="[PROGRAM_CLEANUP]"
cminf="[CMAKE_INFO]"
cmexc="[CMAKE_EXCEPTION]"
cmsuc="[CMAKE_SUCCESS]"
cpinf="[CPACK_INFO]"
cpexc="[CPACK_EXCEPTION]"
cpsuc="[CPACK_SUCCESS]"

# Define paths
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
buildPath="$script_dir/out/build/x64-release"
sourcePath="$script_dir"

# Function to pause
function pause() {
    read -p "$*"
}

# Initialize the environment
echo "$cminf Initializing environment for g++ build..."
if ! command -v g++ &> /dev/null; then
    echo "$prexc g++ is not installed or not in PATH. Please install g++."
    pause "Press Enter to exit..."
    exit 1
fi

# Build the project
build() {
    cd "$buildPath" || exit
    echo "$cminf Started build generation using $numCores cores."
    make -j
    if [ $? -ne 0 ]; then
        echo "$cmexc Build failed. Retrying clean rebuild."
        cmake_configure
    else
        echo "$cmsuc Build succeeded!"
        pause "Press Enter to exit..."
        exit 0
    fi
}

# Configure CMake
cmake_configure() {
    if [ -d "$buildPath" ]; then
        echo "$prcln Removing existing build directory."
        rm -rf "$buildPath"
    fi
    mkdir -p "$buildPath"
    cd "$buildPath" || exit

    echo "$cminf Configuring the project with CMake..."
    cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" "$sourcePath"
    if [ $? -ne 0 ]; then
        echo "$cmexc Configuration failed."
        pause "Press Enter to exit..."
        exit 1
    fi

    build
}

# Main logic
if [ ! -d "$buildPath" ]; then
    echo "$prexc Did not find build folder. Running 'Reconfigure CMake'."
    cmake_configure
else
    build
fi

pause "Press Enter to exit..."
