@echo off
:: Batch script to create the installer for Elypso engine

:: Reusable message types printed to console
set "eninf=[ENGINE_INFO]"
set "enerr=[ENGINE_ERROR]"
set "encle=[ENGINE_CLEANUP]"
set "cpinf=[CPACK_INFO]"
set "cperr=[CPACK_ERROR]"
set "cpsuc=[CPACK_SUCCESS]"

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %enerr% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

:: Change to the script directory
cd /d "%~dp0"

:: Run build.bat if build folder doesn't exist
if not exist build (
	echo %eninf% Did not find build folder. Running build.bat.
	start /wait build.bat skip_pause
)

:: If install folder exists then delete it to always ensure a fresh installer is created
if exist install (
	rd /s /q install
	echo %encle% Deleted folder: install
)

mkdir install

cd build

echo %cpinf% Started CPack configuration.

:: Package the project using CPack with the custom configuration file
cpack -C Release > "logs\cpack_log.txt" 2>&1

if %errorlevel% neq 0 (
	echo %cperr% CPack packaging failed.
) else (
	echo %cpsuc% CPack packaging succeeded!
)

:: Move installer and installed exe to install folder and delete everything else
cd ../install
if exist "%~dp0\install\_CPack_Packages\win64\NSIS\Elypso engine installer.exe" (
	move "%~dp0\install\_CPack_Packages\win64\NSIS\Elypso engine installer\bin\Elypso_engine.exe" "%~dp0\install"
	echo %encle% Moved file: Elypso_engine.exe to install

	move "%~dp0\install\_CPack_Packages\win64\NSIS\Elypso engine installer.exe" "%~dp0\install"
	echo %encle% Moved file: Elypso engine installer.exe to install
	
	rd /s /q "_CPack_Packages"
	echo %encle% Deleted folder: install/_CPack_Packages
) else (
	echo %enerr% Cannot find %~dp0\install\_CPack_Packages\win64\NSIS\Elypso engine installer.exe!
)

pause