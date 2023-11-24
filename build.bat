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
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo [Engine Error] Build failed.
)

:: Additional logging for CMake configuration and CPack
if not exist logs mkdir logs
cmake . > logs\cmake_log.txt 2>&1

:: Package the project using CPack
cpack

if %errorlevel% neq 0 (
    echo [Engine Error] CPack packaging failed.
)

:: Remove unused files and folders in build folder
:: for /f "delims=" %%F in ('dir /b /a-d ^| find /v "Elypso engine-"') do (
::     echo [Engine Cleanup] Deleting file: build/%%F
::     del "%%F"
:: )
:: for /d %%D in (*) do (
::     echo [Engine Cleanup] Deleting folder: build/%%D
::     rd /s /q "%%D"
:: )

pause