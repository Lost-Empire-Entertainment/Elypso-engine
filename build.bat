@echo off
:: Batch script to build the files for Elypso engine

:: Reusable message types printed to console
set "enerr=[ENGINE_ERROR]"
set "encle=[ENGINE_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmerr=[CMAKE_ERROR]"
set "cmsuc=[CMAKE_SUCCESS]"

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %enerr% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

:: Change to the script directory
cd /d "%~dp0"

:: Clean the build directory before configuration
if exist "build" (
    echo %encle% Deleted folder: build
    rd /s /q build
)
mkdir build
cd build

echo %cminf% Started CMake configuration.

:: Configure the project (Release build) and generate NSIS installer
cmake -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR=NSIS ..

if %errorlevel% neq 0 (
    echo %cmerr% CMake configuration failed.
) else (
	echo %cmsuc% Cmake configuration succeeded!
)

:: Build the project
echo %cminf% Started build generation.
mkdir logs
cmake --build . --config Release > logs\build_log.txt 2>&1

if %errorlevel% neq 0 (
    echo %cmerr% Build failed.
) else (
	echo %cmsuc% Build succeeded!
)

:: Allows running "build.bat skip_pause" to build and close console once finished
if "%1" neq "skip_pause" (
    pause
)

exit