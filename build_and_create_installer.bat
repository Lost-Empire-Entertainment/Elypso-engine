@echo off
:: Batch script to build the executable and create the installer for the engine

:: Reusable message types printed to console
set "eninf=[ENGINE_INFO]"
set "enerr=[ENGINE_ERROR]"

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %enerr% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

:: Change to the script directory
cd /d "%~dp0"

echo %eninf% Initializing build generation.

start /wait build.bat build

echo %eninf% Initializing installer creation.

start /wait build.bat install

exit