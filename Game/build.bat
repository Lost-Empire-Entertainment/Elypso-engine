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

set "documentsPath=%USERPROFILE%\Documents\Game"
set "outPath=%~dp0out"
set "vsPath=%~dp0.vs"

set "gamePath=%~dp0build\Release"
set "buildPath=%~dp0build"

:: If no parameter was inserted
if "%~1"=="" (
	goto menu
)

:: Check if first parameter is build
if "%~1"=="build" (
	goto build
) else (
	echo %gexc% Invalid parameter '"%~1"'! Use 'build' to build the game.
	pause
	exit /b 1
)

:menu
cls

echo Game setup
echo.
echo Copyright (C) 2024 Lost Empire Entertainment
echo.
echo This program comes with ABSOLUTELY NO WARRANTY.
echo This is free software, and you are welcome to redistribute it under certain conditions.
echo Read LICENSE.md for more information.

echo.

echo ========================================================

echo.

echo Write the number of your choice to choose the action.
echo.
echo 1. Build Game
echo 2. Exit
echo.
echo 9. Clean Visual Studio (DELETES OUT AND .VS FOLDERS)
echo 0. Clean game (DELETES BUILD AND ENGINE DOCUMENTS FOLDERS)
echo.
set /p choice="Choice: "

:: Process user input
if "%choice%"=="1" goto build
if "%choice%"=="2" exit

if "%choice%"=="9" goto cleanvs
if "%choice%"=="0" goto cleang

echo %prexc% Invalid choice! Please enter a valid number.
pause
goto menu

:cmake
if not "%~1"=="build" (
	cls
)

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
cmake -DCMAKE_BUILD_TYPE=Release ..

if %errorlevel% neq 0 (
	echo %cmexc% CMake configuration failed.
	pause
	goto menu
) else (
	echo %cmsuc% Cmake configuration succeeded!
)
	
goto build

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
	cmake --build . --config Release
	
	if %errorlevel% neq 0 (
		echo %cmexc% Build failed because Game.exe did not get generated properly.
	) else (
		echo %cmsuc% Build succeeded!
	)
)

if "%~1"=="" (
	pause
	goto menu
) else (
	exit
)

:cleanvs
cls

:: Change to the script directory
cd /d "%~dp0"
	
echo %ginf% Running vs clean...
if not exist "%vsPath%" (
	if not exist "%outPath%" (
		echo %gcln% There are no Visual Studio folders to remove.
		pause
		goto menu
	)
)

if exist "%vsPath%" (
	echo %gcln% Deleted folder: .vs
	rd /s /q "%vsPath%"
)
echo "%outPath%"
if exist "%outPath%" (
	echo %gcln% Deleted folder: out
	rd /s /q "%outPath%"
)
	
pause
goto menu

:cleang
cls

:: Change to the script directory
cd /d "%~dp0"
	
if not exist "%buildPath%" (
	if not exist "%documentsPath%" (
		echo %gcln% There are no game folders to remove.
		pause
		goto menu
	)
)

if exist "%buildPath%" (
	echo %encln% Deleted folder: build
	rd /s /q "%buildPath%"
)
if exist "%documentsPath%" (
	echo %gcln% Deleted folder: Documents/Game
	rd /s /q "%documentsPath%"
)
	
pause
goto menu