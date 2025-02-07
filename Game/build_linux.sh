#!/bin/bash

# Define message variables
prexc="[PROGRAM_EXCEPTION]"
prinf="[PROGRAM_INFO]"
prcln="[PROGRAM_CLEANUP]"
cminf="[CMAKE_INFO]"
cmexc="[CMAKE_EXCEPTION]"
cmsuc="[CMAKE_SUCCESS]"

# Define paths
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
sourcePath="$script_dir"

# Initialize number of cores
numCores=$(nproc)

# Validate input parameters
if [ -z "$1" ]; then
    echo "$prexc Empty first parameter detected! Use 'build' or 'cmake'."
    exit 1
fi

if [ "$1" != "build" ] && [ "$1" != "cmake" ]; then
    echo "$prexc Invalid first parameter! Use 'build' or 'cmake'."
    exit 1
fi

if [ -z "$2" ]; then
    echo "$prexc Empty second parameter detected! Use 'release' or 'debug'."
    exit 1
fi

if [ "$2" != "release" ] && [ "$2" != "debug" ]; then
    echo "$prexc Invalid second parameter! Use 'release' or 'debug'."
    exit 1
fi

if [ -n "$3" ] && [ "$3" != "skipwait" ]; then
    echo "$prexc Invalid third parameter! Leave empty or use 'skipwait'."
    exit 1
fi

# Set build path dynamically
if [ "$2" = "debug" ]; then
    buildPath="$script_dir/out/build/x64-debug"
else
    buildPath="$script_dir/out/build/x64-release"
fi

# Function to pause
function pause() {
    if [ "$3" != "skipwait" ]; then
        read -p "Press Enter to exit..."
    fi
}

# Check if g++ is installed
if ! command -v g++ &> /dev/null; then
    echo "$prexc g++ is not installed or not in PATH. Please install g++."
    pause "$1" "$2" "$3"
    exit 1
fi

# Build the project
function build() {
    cd "$buildPath" || exit
    echo "$cminf Started build generation using $numCores cores."
    make -j"$numCores"
    if [ $? -ne 0 ]; then
        echo "$cmexc Build failed."
        pause "$1" "$2" "$3"
        exit 1
    else
        echo "$cmsuc Build succeeded!"
        pause "$1" "$2" "$3"
        exit 0
    fi
}

# Configure CMake
function cmake_configure() {
    if [ -d "$buildPath" ]; then
        echo "$prcln Removing existing build directory."
        rm -rf "$buildPath"
    fi
    mkdir -p "$buildPath"
    cd "$buildPath" || exit

    echo "$cminf Configuring the project with CMake..."
    cmake_build_type=$(echo "$2" | awk '{print toupper(substr($0,1,1))tolower(substr($0,2))}')

    cmake -DCMAKE_C_COMPILER=gcc \
          -DCMAKE_CXX_COMPILER=g++ \
          -DCMAKE_BUILD_TYPE=$cmake_build_type \
          -G "Unix Makefiles" \
          "$sourcePath"
    if [ $? -ne 0 ]; then
        echo "$cmexc Configuration failed."
        pause "$1" "$2" "$3"
        exit 1
    fi

    build "$1" "$2" "$3"
}

# Main logic
case "$1" in
    build)
        if [ ! -d "$buildPath" ]; then
            echo "$prexc Did not find build folder. Running 'Reconfigure CMake'."
            cmake_configure "$1" "$2" "$3"
        else
            build "$1" "$2" "$3"
        fi
        ;;
    cmake)
        cmake_configure "$1" "$2" "$3"
        ;;
    *)
        echo "$prexc Unknown command. Defaulting to 'cmake'."
        cmake_configure "$1" "$2" "$3"
        ;;
esac

pause "$1" "$2" "$3"
