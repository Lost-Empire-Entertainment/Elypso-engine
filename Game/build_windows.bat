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
    echo Empty first parameter detected! Please select build or cmake.
    pause
    exit /b 1
)

:: Parameter 1 can be empty, build or cmake
if NOT "%~1"=="" if NOT "%~1"=="build" if NOT "%~1"=="cmake" (
    echo Invalid first parameter detected! Please select build or cmake.
    pause
    exit /b 1
)

:: Parameter 2 cannot be empty
if "%~2"=="" (
    echo Empty second parameter detected! Please select clang or msvc.
    pause
    exit /b 1
)

:: Parameter 2 must be msvc or clang
if NOT "%~2"=="msvc" if NOT "%~2"=="clang" (
    echo Invalid second parameter selected! Please select clang or msvc.
    pause
    exit /b 1
)

:: Parameter 3 must be empty or skipwait
if NOT "%~3"=="" if NOT "%~3"=="skipwait" (
    echo Invalid third parameter detected! Please leave as empty or select skipwait.
    pause
    exit /b 1
)

:: Launch cl.exe if msvc compiler is selected
if "%~2"=="msvc" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    if %errorlevel% neq 0 (
        echo %prexc% Failed to initialize MSVC environment!
        pause
        exit /b 1
    )
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
    
:: Configures the program if build folder doesn't exist.
if not exist "%buildPath%" goto cmake

cd /d "%buildPath%"
		
:: Build the project
echo %cminf% Started build generation using %numCores% cores.
cmake --build . -- -j%numCores%
if %errorlevel% neq 0 (
	echo %cmexc% Build failed.
	if not "%~3"=="skipwait" (
		pause
	)
    exit /b 0
) else (
    echo %cmsuc% Build succeeded!
	if not "%~3"=="skipwait" (
		pause
	)
	exit /b 0
)

:cmake
:: Change to the build folder
if exist "%buildPath%" (
    rd /S /Q "%buildPath%"
)
mkdir "%buildPath%"
if %errorlevel% neq 0 (
    echo %prcln% Failed to create build directory!
    pause
    exit /b 1
)
cd /d "%buildPath%"

:: Configure the project
if "%~2"=="clang" (
    cmake --preset clang-x64-release -S "%sourcePath%"
    if %errorlevel% neq 0 (
		echo %cmexc% Configuration failed.
		if not "%~3"=="skipwait" (
			pause
		)
		exit /b 1
	)
)
if "%~2"=="msvc" (
    cmake --preset msvc-x64-release -S "%sourcePath%"
    if %errorlevel% neq 0 (
		echo %cmexc% Configuration failed.
		if not "%~3"=="skipwait" (
			pause
		)
		exit /b 1
	)
)
goto build
