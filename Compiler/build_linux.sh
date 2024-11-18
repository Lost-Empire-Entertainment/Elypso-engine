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

# Get the script directory
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
buildPath="$script_dir/build"

# Navigate to the script directory
cd "$script_dir"

# Function to pause
function pause() {
    read -p "$*"
}

# Build the project
if [ ! -d "$buildPath" ]; then
    echo "$prexc Did not find build folder. Running 'Reconfigure CMake'."
    goto_cmake=1
else
    cd "$buildPath"
	
    # Build the project
    echo "$cminf Started build generation."
    make -j$(nproc)
	
    if [ $? -ne 0 ]; then
        echo "$cmexc Build failed because the compiler did not generate properly. Retrying clean rebuild."
        goto_cmake=1
    else
        echo "$cmsuc Build succeeded!"
    fi
fi

if [ "$goto_cmake" == "1" ]; then
    # Remove build directory if it exists
    if [ -d "$buildPath" ]; then
        rm -rf "$buildPath"
    fi
    mkdir "$buildPath"
    cd "$buildPath"
	
    # Configure the project
    cmake -DCMAKE_BUILD_TYPE=Release ..
	
    # Build the project
    echo "$cminf Started build generation."
    make clean
    make -j$(nproc)
	
    if [ $? -ne 0 ]; then
        echo "$cmexc Build failed because the compiler did not generate properly."
    else
        echo "$cmsuc Build succeeded!"
    fi
fi

pause "Press Enter to exit..."