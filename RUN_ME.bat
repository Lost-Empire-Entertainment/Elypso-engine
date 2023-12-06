@echo off
:: Batch script to choose whether to configure, build or install Elypso engine

:: Reusable message types printed to console
set "eninf=[ENGINE_INFO]"
set "enerr=[ENGINE_ERROR]"

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %enerr% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

:: Change to the script directory
cd /d "%~dp0"

:menu
cls
echo Write the number of your choice to choose the action.
echo.
echo 1. Reconfigure CMake
echo 2. Generate build files
echo 3. Generate installer
echo 4. Exit
echo.
echo 0. Reset (DELETES OUT, .VS, BUILD AND INSTALL FOLDERS)
echo.
set /p choice="Choice: "

:: Process user input
if "%choice%"=="1" goto cmake
if "%choice%"=="2" goto build
if "%choice%"=="3" goto install
if "%choice%"=="4" goto end
if "%choice%"=="0" goto reset

echo %enerr% Invalid choice! Please enter a valid number.
pause
goto menu

:cmake
echo %eninf% Running CMake configuration...
start /wait build.bat cmake_config
goto menu

:build
echo %eninf% Running build generation...

if not exist build (
	echo %enerr% Did not find build folder. Please run CMake Configuration.
	pause
) else (
	start /wait build.bat build
)
goto menu

:install
echo %eninf% Running installer generation...

if not exist build (
	echo %enerr% Did not find build folder. Please first run CMake Configuration and then Build Generation.
) else (
	start /wait build.bat install
)
goto menu

:reset
echo %eninf% Running reset...
if not exist out (
	if not exist .vs (
		if not exist build (
			if not exist install (
				echo %eninf% There is nothing to delete.
				pause
				goto menu
			)
		)
	)
)

if exist out (
	echo %eninf% Deleted folder: out
	rd /s /q out
)
if exist .vs (
	echo %eninf% Deleted folder: .vs
	rd /s /q .vs
)
if exist build (
	echo %eninf% Deleted folder: build
	rd /s /q build
)
if exist install (
	echo %eninf% Deleted folder: install
	rd /s /q install
)

pause
goto menu

:end
exit