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
    echo [Engine Cleanup] Deleted folder: install
    rd /s /q install
)
mkdir install

:: Clean the build directory before configuration
if exist build (
    echo [Engine Cleanup] Deleted folder: build
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
mkdir logs
cmake --build . --config Release > logs\build_log.txt 2>&1

if %errorlevel% neq 0 (
    echo [Engine Error] Build failed.
)

:: Package the project using CPack with the custom configuration file
cpack -C Release > logs\cpack_log.txt 2>&1

if %errorlevel% neq 0 (
    echo [Engine Error] CPack packaging failed.
)

:: Move installer and installed exe to install folder and delete everything else
cd ../install
move "%~dp0\install\_CPack_Packages\win64\NSIS\Elypso engine installer\bin\Elypso_engine.exe" "%~dp0\install"
echo [Engine Cleanup] Moved file: Elypso_engine.exe to install
move "%~dp0\install\_CPack_Packages\win64\NSIS\Elypso engine installer.exe" "%~dp0\install"
echo [Engine Cleanup] Moved file: Elypso engine installer.exe to install
rd /s /q "_CPack_Packages"
echo [Engine Cleanup] Deleted folder: install/_CPack_Packages

pause