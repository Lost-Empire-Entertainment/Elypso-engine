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

set "buildPath=%~dp0out/build/x64-release"
set "sourcePath=%~dp0"
set "numCores=%NUMBER_OF_PROCESSORS%"

:: Set up the Visual Studio environment if using MSVC
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

if "%~1"=="build" (
    goto build
)

if "%~1"=="cmake" (
    goto cmake
)

:build

:: Change to the script directory
cd /d "%~dp0"
	
if not exist "%buildPath%" (
	echo %prexc% Did not find build folder. Running 'Reconfigure CMake'.
	goto cmake
) else (
	cd /d "%buildPath%"
		
	:: Build the project
	echo %cminf% Started build generation using %numCores% cores.
	cmake --build . -- -j%numCores%
	if %errorlevel% neq 0 (
		echo %cmexc% Build failed.
		if not "%~2"=="skipwait" (
			pause
		)
        exit /b 0
	) else (
        echo %cmsuc% Build succeeded!
		if not "%~2"=="skipwait" (
			pause
		)
        exit /b 0
    )
)

pause
exit /b

:cmake
:: Change to the build folder
if exist "%buildPath%" (
    rd /S /Q "%buildPath%"
)
mkdir "%buildPath%"
cd /d "%buildPath%"

:: For MinGW
::cmake --preset mingw-x64-release -S "%sourcePath%"

:: For MSVC
cmake --preset msvc-x64-release -S "%sourcePath%"

if %errorlevel% neq 0 (
    echo %cmexc% Configuration failed.
    if not "%~2"=="skipwait" (
		pause
	)
    exit /b 1
)
goto build
