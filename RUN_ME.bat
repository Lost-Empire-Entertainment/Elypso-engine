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
echo 1. Cmake Configuration
echo 2. Build Generation
echo 3. NSIS Installation
echo 4. Exit
echo.
set /p choice="Choice: "

:: Process user input
if "%choice%"=="1" goto cmake_config
if "%choice%"=="2" goto build_generation
if "%choice%"=="3" goto nsis_install
if "%choice%"=="4" goto end

echo %enerr% Invalid choice! Please enter a number between 1 and 4.
pause
goto menu

:cmake_config
echo %eninf% Running CMake configuration...
start /wait build.bat cmake_config
goto menu

:build_generation
echo %eninf% Running build generation...

if not exist build (
	echo %enerr% Did not find build folder. Please run CMake Configuration.
	pause
) else (
	start /wait build.bat build
)
pause
goto menu

:nsis_install
echo %eninf% Running installer generation...

if not exist build (
	echo %enerr% Did not find build folder. Please first run CMake Configuration and then Build Generation.
) else (
	start /wait build.bat install
)
pause
goto menu

:end
exit