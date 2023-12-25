:: Copyright (C) 2023 Greenlaser
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md and EULA.md for more information.

@echo off
:: Batch script to choose whether to configure, build or install Elypso engine

:: Reusable message types printed to console
set "eninf=[ENGINE_INFO]"
set "enexc=[ENGINE_EXCEPTION]"
set "encln=[ENGINE_CLEANUP]"

set "documentsPath=%USERPROFILE%\Documents\Elypso engine"

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %enexc% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

:: Change to the script directory
cd /d "%~dp0"

:menu
cls

echo Elypso engine source manager
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
echo 2. Build engine
echo 3. Exit
echo.
echo 9. Clean Visual Studio (DELETES OUT FOLDER AND .VS FOLDER)
echo 0. Clean Engine (DELETES BUILD FOLDER AND ENGINE DOCUMENTS FOLDER)
echo.
set /p choice="Choice: "

:: Process user input
if "%choice%"=="1" goto cmake
if "%choice%"=="2" goto build
if "%choice%"=="3" goto exit
if "%choice%"=="9" goto cleanvs
if "%choice%"=="0" goto cleanen

echo %enexc% Invalid choice! Please enter a valid number.
pause
goto menu

:cmake
echo %eninf% Running CMake configuration...

start /wait build.bat cmake_config

pause
goto menu

:build
echo %eninf% Building engine...

if not exist build (
	echo Did not find build folder. Please run 'Reconfigure CMake' before building.
) else (
	start /wait build.bat build
)

pause
goto menu

:cleanvs
echo %eninf% Running vs clean...
if not exist out (
	if not exist .vs (
		echo %encln% There is nothing to reset.
		pause
		goto menu
	)
)

if exist out (
	echo %encln% Deleted folder: out
	rd /s /q out
)
if exist .vs (
	echo %encln% Deleted folder: .vs
	rd /s /q .vs
)

pause
goto menu

:cleanen

if not exist build (
	if not exist "%documentsPath%" (
		echo %encln% There is nothing to reset.
		pause
		goto menu
	)
)

if exist build (
	echo %encln% Deleted folder: build
	rd /s /q build
)
if exist "%documentsPath%" (
	echo %encln% Deleted folder: Documents/Elypso engine
	rd /s /q "%documentsPath%"
)

pause
goto menu

:end
exit