:: Copyright (C) 2023 Greenlaser
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md and EULA.md for more information.

@echo off
:: Batch script to build the executable and create the installer for the engine

:: Reusable message types printed to console
set "enexc=[ENGINE_EXCEPTION]"
set "eninf=[ENGINE_INFO]"
set "encln=[ENGINE_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmerr=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"
set "cpinf=[CPACK_INFO]"
set "cperr=[CPACK_EXCEPTION]"
set "cpsuc=[CPACK_SUCCESS]"

set "logsPath=%~dp0logs"

:: Can not run build.bat if no command was inserted
if "%1%" == "" (
	echo %enexc% Please run RUN_ME.bat to choose what actions to do with this project.
	pause
	exit
)

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %enexc% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

::
:: START CMAKE CONFIGURATION WITH "build.bat cmake_config" COMMAND
::

if "%1" == "cmake_config" (
	:: Change to the script directory
	cd /d "%~dp0"

	:: Clean the build directory before configuration
	if exist "build" (
		echo %encln% Deleted folder: build
		rd /s /q build
	)
	mkdir build
	cd build

	echo %cminf% Started CMake configuration.

	:: Configure the project (Release build)
	if not exist "%logsPath%" mkdir "%logsPath%"
	cmake -DCMAKE_BUILD_TYPE=Release .. >> "%logsPath%\cmake_log.txt" 2>&1

	if %errorlevel% neq 0 (
		echo %cmerr% CMake configuration failed. Check logs/build_log.txt for more details.
	) else (
		echo %cmsuc% Cmake configuration succeeded! Created log file at logs/cmake_log.txt.
	)
)

::
:: START BUILD WITH "build.bat build" COMMAND
::

if "%1" == "build" (
	cd /d "C:\Program Files"
	
	:: Change to the script directory
	cd /d "%~dp0\build"

	:: Build the project
	echo %cminf% Started build generation.
	if not exist "%logsPath%" mkdir "%logsPath%"
	cmake --build . --config Release > "%logsPath%\build_log.txt" 2>&1
	
	if %errorlevel% neq 0 (
		echo %cmerr% Build failed because Elypso_engine.exe did not get generated properly. Check logs/build_log.txt for more details.
	) else (
		echo %cmsuc% Build succeeded! Created log file at logs/build_log.txt.
	)
)

pause
exit