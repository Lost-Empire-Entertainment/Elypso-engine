:: Copyright (C) 2024 Greenlaser
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

set "documentsPath=%USERPROFILE%\Documents\Elypso engine"
set "outPath=%~dp0out"
set "vsPath=%~dp0.vs"

set "buildPath=%~dp0build"
:: Can not run build.bat without administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
	echo %enexc% This script requires administrative privileges. Please run as administrator.
	pause
	exit /b 1
) else (
	:: Configures CMake and creates the build folder
	if "%1" == "cmake" (
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
		cmake -DCMAKE_BUILD_TYPE=Release ..

		if %errorlevel% neq 0 (
			echo %cmerr% CMake configuration failed.
		)
	
		pause
		exit
	)

	:: Creates the executable to build\Release
	if "%1" == "build" (
		:: Change to the script directory
		cd /d "%~dp0"
	
		if not exist "%buildPath%" (
			echo Did not find build folder. Please run 'Reconfigure CMake' before building.
			pause
			exit
		) else (
			cd "%buildPath%"
		
			:: Build the project
			echo %cminf% Started build generation.
			cmake --build . --config Release
	
			if %errorlevel% neq 0 (
				echo %cmerr% Build failed because Elypso_engine.exe did not get generated properly.
			)
			
			pause
			exit
		)
	)

	:: Deletes Visual Studio .vs and out folders
	if "%1" == "cleanvs" (
		:: Change to the script directory
		cd /d "%~dp0"
	
		echo %eninf% Running vs clean...
		if not exist "%vsPath%" (
			if not exist "%outPath%" (
				echo %encln% There are no Visual Studio folders to remove.
				pause
				exit
			)
		)

		if exist "%vsPath%" (
			echo %encln% Deleted folder: .vs
			rd /s /q "%vsPath%"
		)
		echo "%outPath%"
		if exist "%outPath%" (
			echo %encln% Deleted folder: out
			rd /s /q "%outPath%"
		)
	
		pause
		exit
	)

	:: Deletes engine build and engine documents folders
	if "%1" == "cleanen" (
		:: Change to the script directory
		cd /d "%~dp0"
	
		if not exist "%buildPath%" (
			if not exist "%documentsPath%" (
				echo %encln% There are no engine folders to remove.
				pause
				exit
			)
		)

		if exist "%buildPath%" (
			echo %encln% Deleted folder: build
			rd /s /q "%buildPath%"
		)
		if exist "%documentsPath%" (
			echo %encln% Deleted folder: Documents/Elypso engine
			rd /s /q "%documentsPath%"
		)
	
		pause
		exit
	)
)

echo %enexc% Please run Source_manager.exe to use the mechanics of this bat script.

pause
exit