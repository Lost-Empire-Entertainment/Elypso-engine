:: Copyright (C) 2024 Greenlaser
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md and EULA.md for more information.

@echo off
:: Batch script to build the executable and create the installer for Elypso hub

:: Reusable message types printed to console
set "enexc=[HUB_EXCEPTION]"
set "eninf=[HUB_INFO]"
set "encln=[HUB_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmerr=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"
set "cpinf=[CPACK_INFO]"
set "cperr=[CPACK_EXCEPTION]"
set "cpsuc=[CPACK_SUCCESS]"

set "documentsPath=%USERPROFILE%\Documents\Elypso hub"
set "outPath=%~dp0out"
set "vsPath=%~dp0.vs"

set "buildPath=%~dp0build"

:: Can not run build.bat without administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
	echo %enexc% This script requires administrative privileges. Please run as administrator.
	pause
	exit /b 1
)

:menu
cls

echo Elypso hub setup
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
echo 2. Build Elypso hub
echo 3. Exit
echo.
echo 9. Clean Visual Studio (DELETES OUT AND .VS FOLDERS)
echo 0. Clean hub (DELETES BUILD AND HUB DOCUMENTS FOLDERS)
echo.
set /p choice="Choice: "

:: Process user input
if "%choice%"=="1" goto cmake
if "%choice%"=="2" goto build
if "%choice%"=="3" exit

if "%choice%"=="9" goto cleanvs
if "%choice%"=="0" goto cleanhub

echo %prexc% Invalid choice! Please enter a valid number.
pause
goto menu

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
	cd "%buildPath%"
		
	:: Build the project
	echo %cminf% Started build generation.
	cmake --build . --config Release
	
	if %errorlevel% neq 0 (
		echo %cmerr% Build failed because Elypso hub.exe did not get generated properly.
	) else (
		echo %cmsuc% Build succeeded!
	)
)

pause
goto menu

:cleanvs
:: Change to the script directory
cd /d "%~dp0"
	
echo %eninf% Running vs clean...
if not exist "%vsPath%" (
	if not exist "%outPath%" (
		echo %encln% There are no Visual Studio folders to remove.
		pause
		goto menu
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
goto menu

:cleanhub
:: Change to the script directory
cd /d "%~dp0"
	
if not exist "%buildPath%" (
	if not exist "%documentsPath%" (
		echo %encln% There are no hub folders to remove.
		pause
		goto menu
	)
)

if exist "%buildPath%" (
	echo %encln% Deleted folder: build
	rd /s /q "%buildPath%"
)
if exist "%documentsPath%" (
	echo %encln% Deleted folder: Documents/Elypso hub
	rd /s /q "%documentsPath%"
)
	
pause
goto menu