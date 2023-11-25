@echo off
:: Batch script to configure, build, and create installer for Elypso engine

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

:: Change to the script directory
cd /d "%~dp0"

:: Clean the install directory before configuration
if exist install (
    echo [Engine Cleanup] Deleting folder: install
    rd /s /q install
)
mkdir install

:: Clean the build directory before configuration
if exist build (
    echo [Engine Cleanup] Deleting folder: build
    rd /s /q build
)
mkdir build
cd build

:: Configure the project (Release build) and generate NSIS installer
cmake -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR=NSIS ..

if %errorlevel% neq 0 (
    echo [Engine Error] CMake configuration failed.
)

:: Build the project
if not exist logs mkdir logs
cmake --build . --config Release > logs\build_log.txt 2>&1

if %errorlevel% neq 0 (
    echo [Engine Error] Build failed.
)

cmake . > logs\cmake_log.txt 2>&1

:: Package the project using CPack
cpack > logs\cpack_log.txt 2>&1

if %errorlevel% neq 0 (
    echo [Engine Error] CPack packaging failed.
)

pause