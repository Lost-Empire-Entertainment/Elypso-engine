:: Copyright (C) 2024 Lost Empire Entertainment
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md for more information.

@echo off
:: Batch script to build the executable and create the installer for the game

:: Reusable message types printed to console
set "gcln=[GAME_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmerr=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"

if "%~1"=="" (
    echo Error: No build path was provided!
	exit /b 1
) else (
	set "parentPath=%~1"
    set "buildPath=%~1\build"
)

if not exist "%buildPath%" (
	goto cmake
) else (
	goto cmake
	
	:: gotta figure out how to go straight to build
	:: right now it gives error "Error: could not load cache"
	:: if compiling in engine
	:: goto build
)

:cmake

if exist "%buildPath%" (
	echo %gcln% Deleted folder: build
	rd /s /q "%buildPath%"
)

mkdir "%buildPath%"
cd /d "%buildPath%"

echo %cminf% Started CMake configuration.

:: Configure the project (Release build)
cmake -DCMAKE_BUILD_TYPE=Release ..

if %errorlevel% neq 0 (
	echo %cmerr% CMake configuration failed.
) else (
	echo %cmsuc% Cmake configuration succeeded!
)

goto build

:build
	
:: Build the project
echo %cminf% Started build generation.
cmake --build . --config Release
	
if %errorlevel% neq 0 (
	echo %cmerr% Build failed because Game.exe did not get generated properly.
) else (
	echo %cmsuc% Build succeeded!
)

exit