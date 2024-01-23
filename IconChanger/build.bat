:: Copyright (C) 2024 Greenlaser
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md and EULA.md for more information.

@echo off
:: Batch script that handles CMake configuration and executable creation for the Elypso engine Source manager

:: Reusable message types printed to console
set "prexc=[PROGRAM_EXCEPTION]"
set "prinf=[PROGRAM_INFO]"
set "prcln=[PROGRAM_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmerr=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"
set "cpinf=[CPACK_INFO]"
set "cperr=[CPACK_EXCEPTION]"
set "cpsuc=[CPACK_SUCCESS]"

set "outPath=%~dp0out"
set ".vsPath=%~dp0.vs"
set "buildPath=%~dp0build"

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %prexc% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

:menu
cls

echo Elypso engine IconChanger setup
echo.
echo Copyright (C) 2024 Greenlaser
echo.
echo This program comes with ABSOLUTELY NO WARRANTY.
echo This is free software, and you are welcome to redistribute it under certain conditions.
echo Read LICENSE.md and EULA.md for more information.

echo.

echo ========================================================

echo.

echo Write the number of your choice to choose the action.
echo.
echo 1. Reconfigure CMake
echo 2. Build IconChanger
echo 3. Exit
echo.
echo 0. Reset (DELETES BUILD, OUT AND .VS FOLDERS)
echo.
set /p choice="Choice: "

:: Process user input
if "%choice%"=="1" goto cmake
if "%choice%"=="2" goto build
if "%choice%"=="3" goto exit
if "%choice%"=="0" goto cleanvs

echo %prexc% Invalid choice! Please enter a valid number.
pause
goto menu

:cmake
:: Change to the script directory
cd /d "%~dp0"

:: Clean the build directory before configuration
if exist "%buildPath%" (
	echo %prcln% Deleted folder: build
	rd /s /q "%buildPath%"
)
mkdir "%buildPath%"
cd "%buildPath%"

echo %cminf% Started CMake configuration.

:: Configure the project (Release build)
cmake -DCMAKE_BUILD_TYPE=Release ..

if %errorlevel% neq 0 (
	echo %cmerr% CMake configuration failed.
) else (
	echo %cmsuc% Cmake configuration succeeded!
)

pause
goto menu

:build
:: Change to the script directory
cd /d "%~dp0"

if not exist "%buildPath%" (
	echo Did not find build folder. Please run 'Reconfigure CMake' before building.
) else (
	:: Change to the script directory
	cd /d "%buildPath%"

	:: Build the project
	echo %cminf% Started build generation.
	cmake --build . --config Release
	
	if %errorlevel% neq 0 (
		echo %cmerr% Build failed because IconChanger.exe did not get generated properly.
	) else (
		echo %cmsuc% Build succeeded!
	)
)

pause
goto menu

:cleanvs
:: Change to the script directory
cd /d "%~dp0"

echo %prinf% Running vs clean...
if not exist "%buildPath%" (
	if not exist "%outPath%" (
		if not exist "%vsPath%" (
			echo %prcln% There is nothing to reset.
			pause
			goto menu
		)
	)
)

if exist "%buildPath%" (
	echo %prcln% Deleted folder: build
	rd /s /q "%buildPath%"
)
if exist "%outPath%" (
	echo %prcln% Deleted folder: out
	rd /s /q "%outPath%"
)
if exist "%vsPath%" (
	echo %prcln% Deleted folder: .vs
	rd /s /q "%vsPath%"
)

pause
goto menu

:end
exit