:: Copyright (C) 2023 Greenlaser
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md and EULA.md for more information.

@echo off
:: Front-end batch script for the Source manager setup build.bat file

:: Reusable message types printed to console
set "prinf=[PROGRAM_INFO]"
set "prexc=[PROGRAM_EXCEPTION]"
set "prcln=[PROGRAM_CLEANUP]"

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %prexc% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

:: Change to the script directory
cd /d "%~dp0"

:menu
cls

echo Elypso engine Source manager setup
echo.
echo Copyright (C) 2023 Greenlaser
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
echo 2. Build Source manager
echo 3. Exit
echo.
echo 0. Reset (DELETES BUILD, OUT AND .VS FOLDERS)
echo.
set /p choice="Choice: "

:: Process user input
if "%choice%"=="1" goto cmake
if "%choice%"=="2" goto build
if "%choice%"=="3" goto exit
if "%choice%"=="0" goto reset

echo %prexc% Invalid choice! Please enter a valid number.
pause
goto menu

:cmake
echo %prinf% Running CMake configuration...

start /wait build.bat cmake_config

pause
goto menu

:build
echo %prinf% Building engine...

if not exist build (
	echo Did not find build folder. Please run 'Reconfigure CMake' before building.
) else (
	start /wait build.bat build
)

pause
goto menu

:reset
echo %prinf% Running reset...
if not exist build (
	if not exist out (
		if not exist .vs (
			echo %prcln% There is nothing to reset.
			pause
			goto menu
		)
	)
)

if exist build (
	echo %prcln% Deleted folder: build
	rd /s /q build
)
if exist out (
	echo %prcln% Deleted folder: out
	rd /s /q out
)
if exist .vs (
	echo %prcln% Deleted folder: .vs
	rd /s /q .vs
)

pause
goto menu

:end
exit