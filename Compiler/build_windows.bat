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

:: Initialize Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if %errorlevel% neq 0 (
    echo %prexc% Failed to initialize Visual Studio environment.
    pause
    exit /b 1
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
		echo %cmexc% Build failed. Retrying clean rebuild.
		goto cmake
	) else (
        echo %cmsuc% Build succeeded!
        pause
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

:: Configure the project
cmake --preset x64-release -S "%sourcePath%"
if %errorlevel% neq 0 (
    echo %cmexc% Configuration failed.
    pause
    exit /b 1
)
goto build
