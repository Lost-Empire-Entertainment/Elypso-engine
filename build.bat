@echo off
:: Batch script to configure, build, and install Elypso engine

:: Change to the script directory
cd /d "%~dp0"

:: Remove existing build and bin directories
rmdir /s /q build
rmdir /s /q bin

:: Configure the project (Release build)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

:: Build the project
cmake --build build --config Release

:: Install the project
cmake --install build --config Release

:: Additional logging for CMake configuration and CPack
cmake . > cmake_log.txt 2>&1
cpack > cpack_log.txt 2>&1

:: Pause to keep the console window open for review
pause