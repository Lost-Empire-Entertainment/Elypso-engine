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

:: Parameter 3 cannot be empty
if "%~3"=="" (
    echo Empty third parameter detected! Please select release or debug.
    pause
    exit /b 1
)

:: Parameter 3 must be release or debug
if NOT "%~3"=="release" if NOT "%~3"=="debug" (
    echo Invalid third parameter selected! Please select release or debug.
    pause
    exit /b 1
)

:: Parameter 4 must be empty or skipwait
if NOT "%~4"=="" if NOT "%~4"=="skipwait" (
    echo Invalid third parameter detected! Please leave as empty or select skipwait.
    pause
    exit /b 1
)

:: Launch cl.exe if msvc compiler is selected and set build path to msvc-x64-release
if "%~2"=="msvc" (
	set "buildPath=%~dp0out/build/msvc-x64-release"
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    if %errorlevel% neq 0 (
        echo %prexc% Failed to initialize MSVC environment!
        pause
        exit /b 1
    )
)

:: Set clang build path
if "%~2"=="clang" (
	if "%~3"=="release" (
		set "buildPath=%~dp0out/build/clang-x64-release"
	)
	if "%~3"=="debug" (
		set "buildPath=%~dp0out/build/clang-x64-debug"
	)
)

:: Set msvc build path
if "%~2"=="msvc" (
	if "%~3"=="release" (
		set "buildPath=%~dp0out/build/msvc-x64-release"
	)
	if "%~3"=="debug" (
		set "buildPath=%~dp0out/build/msvc-x64-debug"
	)
)

:: Either builds or sets up cmake.
if "%~1"=="build" (
    goto build
)
if "%~1"=="" (
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
	if not "%~4"=="skipwait" (
		pause
	)
    exit /b 0
) else (
    echo %cmsuc% Build succeeded!
	if not "%~4"=="skipwait" (
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
	if "%~3"=="release" (
		cmake --preset clang-x64-release -S "%sourcePath%"
		if %errorlevel% neq 0 (
			echo %cmexc% Configuration failed.
			if not "%~4"=="skipwait" (
				pause
			)
			exit /b 1
		)
	)
	if "%~3"=="debug" (
		cmake --preset clang-x64-debug -S "%sourcePath%"
		if %errorlevel% neq 0 (
			echo %cmexc% Configuration failed.
			if not "%~4"=="skipwait" (
				pause
			)
			exit /b 1
		)
	)
)
if "%~2"=="msvc" (
    if "%~3"=="release" (
		cmake --preset msvc-x64-release -S "%sourcePath%"
		if %errorlevel% neq 0 (
			echo %cmexc% Configuration failed.
			if not "%~4"=="skipwait" (
				pause
			)
			exit /b 1
		)
	)
	if "%~3"=="debug" (
		cmake --preset msvc-x64-debug -S "%sourcePath%"
		if %errorlevel% neq 0 (
			echo %cmexc% Configuration failed.
			if not "%~4"=="skipwait" (
				pause
			)
			exit /b 1
		)
	)
)
goto build
