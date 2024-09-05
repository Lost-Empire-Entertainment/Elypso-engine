:: Copyright (C) 2024 Lost Empire Entertainment
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md for more information.

@echo off
:: Batch script to build the executable and create the installer for the engine

title Build Elypso Engine

:: Reusable message types printed to console
set "enexc=[ENGINE_EXCEPTION]"
set "eninf=[ENGINE_INFO]"
set "cminf=[CMAKE_INFO]"
set "cmexc=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"
set "cpinf=[CPACK_INFO]"
set "cpexc=[CPACK_EXCEPTION]"
set "cpsuc=[CPACK_SUCCESS]"

set "buildPath=%~dp0build"

:build

:: Change to the script directory
cd /d "%~dp0"
	
if not exist "%buildPath%" (
	echo %enexc% Did not find build folder. Running 'Reconfigure CMake'.
		
	goto cmake
) else (
	cd "%buildPath%"
		
	:: Build the project
	echo %cminf% Started build generation.
	cmake --build . --config Release -- /m
	
	if %errorlevel% neq 0 (
		echo %cmexc% Build failed because Elypso engine.exe did not get generated properly.
	) else (
		echo %cmsuc% Build succeeded!
		pause
		exit
	)
)

:cmake
:: Change to the script directory
cd /d "%~dp0"
	
:: Clean the build directory before configuration
if exist "%buildPath%" (
	echo %encln% Deleted folder: build
	rd /s /q "%buildPath%"
)
mkdir "%buildPath%"
cd "%buildPath%"

echo %cminf% Started CMake configuration.

:: Configure the project (Release build)
cmake -A x64 ..

if %errorlevel% neq 0 (
	echo %cmexc% CMake configuration failed.
	pause
) else (
	echo %cmsuc% Cmake configuration succeeded!
	goto build
)