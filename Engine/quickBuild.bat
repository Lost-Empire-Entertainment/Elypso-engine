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

set waitAtEnd=%1
::waitAtEnd parameter was empty
if "%waitAtEnd%"=="" (
	echo Wait at end parameter was not provided so it was set to 'n'.
	set waitAtEnd=n
)
::waitAtEnd parameter was invalid
if not "%waitAtEnd%"=="y" (
	if not "%waitAtEnd%"=="n" (
		echo Provided waitAtEnd parameter '"%waitAtEnd%"' is invalid! Please use 'y' or 'n'!
		exit /b 1
	)
)

set fullRebuild=%2
::fullRebuild parameter was empty
if "%fullRebuild%"=="" (
	echo Full rebuild parameter was not provided so it was set to 'n'.
	set fullRebuild=n
)
::fullRebuild parameter was invalid
if not "%fullRebuild%"=="y" (
	if not "%fullRebuild%"=="n" (
		echo Provided full rebuild parameter '"%fullRebuild%"' is invalid! Please use 'y' or 'n'!
		exit /b 1
	)
)

set buildType=%3
::buildType parameter was empty
if "%buildType%"=="" (
	echo Build type parameter was not provided so it was set to 'Release'.
	set buildType=Release
)
::buildType parameter was invalid
if not "%buildType%"=="Release" (
	if not "%buildType%"=="Debug" (
		echo Provided buildType parameter '"%buildType%"' is invalid! Please use 'Release' or 'Debug'!
		exit /b 1
	)
)

::switches to :cmake if full rebuild was set to 'y'
if "%fullRebuild%"=="" (
	echo Full rebuild was set to 'y' manually so Elypso engine will be rebuilt from scratch.
	goto cmake
)

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
		if "%waitAtEnd%"=="y" (
			pause
		)
		exit
	) else (
		echo %cmsuc% Build succeeded!
		if "%waitAtEnd%"=="y" (
			pause
		)
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
cmake -A x64 -DCMAKE_BUILD_TYPE=%buildType% ..

if %errorlevel% neq 0 (
	echo %cmexc% CMake configuration failed.
	if "%waitAtEnd%"=="y" (
		pause
	)
	exit
) else (
	echo %cmsuc% Cmake configuration succeeded!
	goto build
)