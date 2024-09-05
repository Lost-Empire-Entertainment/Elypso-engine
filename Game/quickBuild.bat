:: Copyright (C) 2024 Lost Empire Entertainment
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md for more information.

@echo off
:: Batch script to build the executable for the game

title Build Game exe

:: Reusable message types printed to console
set "gexc=[GAME_EXCEPTION]"
set "ginf=[GAME_INFO]"
set "gcln=[GAME_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmexc=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"
set "cpinf=[CPACK_INFO]"
set "cpexc=[CPACK_EXCEPTION]"
set "cpsuc=[CPACK_SUCCESS]"

set "buildPath=%~dp0build"

if "%~1"=="cleanRebuild" (
	goto cmake
)

:build

:: Change to the script directory
cd /d "%~dp0"
	
if not exist "%buildPath%" (
	echo %gexc% Did not find build folder. Running 'Reconfigure CMake'.
	
	goto cmake
) else (
	cd "%buildPath%"
	
	:: Deletes existing Release folder and creates a new one
	if exist "%gamePath%" (
		rd /s /q "%gamePath%"
		mkdir "%gamePath%"
	)
		
	:: Build the project
	echo %cminf% Started build generation.
	cmake --build . --config Release -- /m
	
	if %errorlevel% neq 0 (
		echo %cmexc% Build failed because Game.exe did not get generated properly.
		if not "%~1"=="bypassInput" (
			if not "%~1"=="cleanRebuild" (
				pause
			)
		)
		exit
	) else (
		echo %cmsuc% Build succeeded!
		if not "%~1"=="bypassInput" (
			if not "%~1"=="cleanRebuild" (
				pause
			)
		)
		exit
	)
)

:cmake
:: Change to the script directory
cd /d "%~dp0"
	
:: Clean the build directory before configuration
if exist "%buildPath%" (
	echo %gcln% Deleted folder: build
	rd /s /q "%buildPath%"
)
mkdir "%buildPath%"
cd "%buildPath%"

echo %cminf% Started CMake configuration.

:: Configure the project (Release build)
cmake -A x64 ..

if %errorlevel% neq 0 (
	echo %cmexc% CMake configuration failed.
	if not "%~1"=="bypassInput" (
		if not "%~1"=="cleanRebuild" (
			pause
		)
	)
	exit
) else (
	goto build
	echo %cmsuc% Cmake configuration succeeded!
)