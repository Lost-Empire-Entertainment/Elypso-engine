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
	if not exist logs mkdir logs
	cmake -DCMAKE_BUILD_TYPE=Release .. >> logs\cmake_log.txt 2>&1

	if %errorlevel% neq 0 (
		echo %cmerr% CMake configuration failed. Check build/logs/build_log.txt for more details.
	) else (
		echo %cmsuc% Cmake configuration succeeded! Created log file at build/logs/cmake_log.txt.
	)
)

::
:: START BUILD WITH "build.bat build" COMMAND
::

if "%1" == "build" (
	:: Change to the script directory
	cd /d "%~dp0\build"

	:: Build the project
	echo %cminf% Started build generation.
	cmake --build . --config Release > logs\build_log.txt 2>&1
	
	if %errorlevel% neq 0 (
		echo %cmerr% Build failed because Elypso_engine.exe did not get generated properly. Check build/logs/build_log.txt for more details.
	) else (
		echo %cmsuc% Build succeeded! Created log file at build/logs/build_log.txt.
	)
)

::
:: START INSTALL WITH "build.bat install" COMMAND
::

if "%1" == "install" (
	cd /d "C:\Program Files"
	
	if not exist "Elypso engine" mkdir "Elypso engine"
		
	if exist "Elypso engine\Elypso_engine.exe" (
		echo %encln% Deleted file: Elypso_engine.exe
		del "Elypso engine\Elypso_engine.exe"
	)
	if exist "Elypso engine\files" (
		echo %encln% Deleted folder: files
		rd /s /q "Elypso engine\files"
	)
	if exist "Elypso engine\LICENSE.md" (
		echo %encln% Deleted file: LICENSE.md
		del "Elypso engine\LICENSE.md"
	)
	if exist "Elypso engine\EULA.md" (
		echo %encln% Deleted file: EULA.md
		del "Elypso engine\EULA.md"
	)
	if exist "Elypso engine\README.md" (
		echo %encln% Deleted file: README.md
		del "Elypso engine\README.md"
	)
	
	echo %encln% Copied file: Elypso_engine.exe to C:\Program Files\Elypso engine
	copy "%~dp0\build\Release\Elypso_engine.exe" "C:\Program Files\Elypso engine\Elypso_engine.exe"
	
	echo %encln% Copied folder: files to C:\Program Files\Elypso engine
	xcopy "%~dp0\build\Release\files" "C:\Program Files\Elypso engine\files" /E /I /Y
		
	echo %encln% Copied file: LICENSE.md to C:\Program Files\Elypso engine
	copy "%~dp0\LICENSE.md" "C:\Program Files\Elypso engine\LICENSE.md"
		
	echo %encln% Copied file: EULA.md to C:\Program Files\Elypso engine
	copy "%~dp0\EULA.md" "C:\Program Files\Elypso engine\EULA.md"
		
	echo %encln% Copied file: README.md to C:\Program Files\Elypso engine
	copy "%~dp0\README.md" "C:\Program Files\Elypso engine\README.md"
)

pause
exit