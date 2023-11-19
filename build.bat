@echo off
:: Batch script to configure, build, and create installer for Elypso engine

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

:: Change to the script directory
cd /d "%~dp0"

:: Clean the build directory before configuration
if exist build (
    echo [Engine] Deleting folder: build
    rd /s /q build
)
mkdir build

:: Configure the project (Release build) and generate NSIS installer
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR=NSIS

:: Build the project
cmake --build build --config Release

:: Additional logging for CMake configuration and CPack
if not exist logs mkdir logs
cmake . > logs\cmake_log.txt 2>&1

:: Package the project using CPack
cd build
cpack

:: Remove unused files and folders in build folder
for /f "delims=" %%F in ('dir /b /a-d ^| find /v "Elypso engine-"') do (
    echo [Engine] Deleting file: build/%%F
    del "%%F"
)
for /d %%D in (*) do (
    echo [Engine] Deleting folder: build/%%D
    rd /s /q "%%D"
)

cd ..

:: Remove unused files and folders in source folder
for /f "delims=" %%X in ('dir /b /a-d *.vcxproj *.filters *.sln *.cmake CMakeCache.txt install_manifest.txt') do (
	if exist "%%X" (
		echo [Engine] Deleting file: %%X
        del /q "%%X"
    )
)
for /d %%D in (CMakeFiles _CPACK_Packages) do (
    if exist "%%D" (
        echo [Engine] Deleting folder: %%D
        rd /s /q "%%D"
    )
)

pause