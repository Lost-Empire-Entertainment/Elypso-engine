@echo off
setlocal

set "prexc=[PROGRAM_EXCEPTION]"
set "prinf=[PROGRAM_INFO]"
set "prcln=[PROGRAM_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmexc=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"

set "rootDir=%~dp0"
set "numCores=%NUMBER_OF_PROCESSORS%"

:: Validate input parameters
if "%~1"=="" (
    echo [ERROR] Empty first parameter detected! Use 'build' or 'cmake'.
    if not "%~3"=="skipwait" pause
    exit /b 1
)

if NOT "%~1"=="build" if NOT "%~1"=="cmake" (
    echo [ERROR] Invalid first parameter! Use 'build' or 'cmake'.
    if not "%~3"=="skipwait" pause
    exit /b 1
)

if "%~2"=="" (
    echo [ERROR] Empty second parameter detected! Use 'release' or 'debug'.
    if not "%~3"=="skipwait" pause
    exit /b 1
)

if NOT "%~2"=="release" if NOT "%~2"=="debug" (
    echo [ERROR] Invalid second parameter! Use 'release' or 'debug'.
    if not "%~3"=="skipwait" pause
    exit /b 1
)

if NOT "%~3"=="" if NOT "%~3"=="skipwait" (
    echo [ERROR] Invalid third parameter! Leave empty or use 'skipwait'.
    pause
    exit /b 1
)

:: Record start time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_START=%%a:%%b:%%c"

:: Set build path dynamically
set "buildPath=%rootDir%out/build/x64-%~2"

:: Set up MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if %errorlevel% neq 0 (
    echo %prexc% Failed to initialize MSVC environment!
    if not "%~3"=="skipwait" pause
    exit /b 1
)

:: Execute build or CMake configuration
if "%~1"=="cmake" goto cmake

:build
cd /d "%rootDir%"

:: Configure first if the build folder does not exist
if not exist "%buildPath%" goto cmake

cd /d "%buildPath%"
echo %cminf% Started build generation using %numCores% cores.
cmake --build . -- -j%numCores%
if %errorlevel% neq 0 (
    echo %cmexc% Build failed.
    if not "%~3"=="skipwait" pause
    exit /b 1
)

:: Record end time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_END=%%a:%%b:%%c"

echo %cmsuc% Build succeeded!
echo ---------------------------------------------
echo Build duration: %TIME_START% - %TIME_END%
echo ---------------------------------------------

if not "%~3"=="skipwait" pause
exit /b 0

:cmake
:: Remove old build directory if it exists
if exist "%buildPath%" rd /S /Q "%buildPath%"
mkdir "%buildPath%"
if %errorlevel% neq 0 (
    echo %prcln% Failed to create build directory!
    if not "%~3"=="skipwait" pause
    exit /b 1
)

cd /d "%buildPath%"

:: Configure the project
cmake --preset x64-%~2 -S "%rootDir%"
if %errorlevel% neq 0 (
    echo %cmexc% Configuration failed.
    if not "%~3"=="skipwait" pause
    exit /b 1
)

goto build
