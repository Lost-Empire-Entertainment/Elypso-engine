:: Copyright (C) 2024 Lost Empire Entertainment
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md for more information.

@echo off
:: Batch script to build the executable for the game, intended for use with the engine only.

:: Reusable message types printed to console
set "gcln=[GAME_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmerr=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"

set "buildPath=%~dp0build"

:cmake
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
cmake -DCMAKE_BUILD_TYPE=Release ..

if %errorlevel% neq 0 (
	echo %cmerr% CMake configuration failed.
	pause
	goto menu
) else (
	echo %cmsuc% Cmake configuration succeeded!
)
	
goto build
	
:build 
cd /d "%~dp0"
	
cd "%buildPath%"
		
:: Build the project
echo %cminf% Started build generation.
cmake --build . --config Release
	
if %errorlevel% neq 0 (
	echo %cmerr% Build failed because Game.exe did not get generated properly.
) else (
	echo %cmsuc% Build succeeded!
)

exit