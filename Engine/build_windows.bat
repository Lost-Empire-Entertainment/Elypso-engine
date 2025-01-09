@echo off

set "prexc=[PROGRAM_EXCEPTION]"
set "prinf=[PROGRAM_INFO]"
set "prcln=[PROGRAM_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmexc=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"
set "cpinf=[CPACK_INFO]"
set "cpexc=[CPACK_EXCEPTION]"
set "cpsuc=[CPACK_SUCCESS]"

set "rootDir=%~dp0"
set "buildPath=%~dp0out/build/x64-release"
set "sourcePath=%~dp0"
set "numCores=%NUMBER_OF_PROCESSORS%"

:: Parameter 1 cannot be empty
if "%~1"=="" (
	echo Action has not been selected! Please select none, build or cmake as first parameter.
    pause
	exit /b 1
)

:: Parameter 1 can be empty, build or cmake
if NOT "%~1"=="" if NOT "%~1"=="build" if NOT "%~1"=="cmake" (
    echo Invalid command selected! Please choose either build, cmake, or leave it empty.
    pause
    exit /b 1
)

:: Parameter 2 cannot be empty
if "%~2"=="" (
	echo Compiler has not been selected! Please select clang or msvc as second parameter.
    pause
	exit /b 1
)

:: Parameter 2 must be msvc or clang
if NOT "%~2"=="msvc" if NOT "%~2"=="clang" (
    echo Invalid compiler selected! Please choose either clang or msvc.
    pause
    exit /b 1
)

:: Launch cl.exe if msvc compiler is selected
if "%~2"=="msvc" (
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
)

:: Either builds or sets up cmake.
if "%~1"=="build" (
    goto build
)
if "%~1"=="none" (
    goto build
)
if "%~1"=="cmake" (
    goto cmake
)

:build
cd /d "%rootDir%"
	
:: Configures the program if build folder doesnt exist.
if not exist "%buildPath%" goto cmake

cd /d "%buildPath%"
		
:: Build the project
echo %cminf% Started build generation using %numCores% cores.
call :run "cmake --build . -- -j%numCores%"
echo %cmsuc% Build succeeded!
if not "%~2"=="skipwait" (
	pause
)
exit /b 0

pause
exit /b

:cmake
:: Change to the build folder
if exist "%buildPath%" (
    rd /S /Q "%buildPath%"
)
mkdir "%buildPath%"
cd /d "%buildPath%"

:: Configure the project
if "%~2"=="clang" (
	call :run "cmake --preset clang-x64-release -S "%sourcePath%""
)
if "%~2"=="msvc" (
	call :run "cmake --preset msvc-x64-release -S "%sourcePath%""
)
goto build

:: Error handling function
:run
%*
if %errorlevel% neq 0 (
    echo %prexc% %~nx1 failed!
    exit /b 1
)
exit /b 0