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

set "buildPath=%~dp0build"

:build

:: Change to the script directory
cd /d "%~dp0"
	
if not exist "%buildPath%" (
	echo %prexc% Did not find build folder. Running 'Reconfigure CMake'.
	goto cmake
) else (
	cd /d "%buildPath%"
		
	:: Build the project
	echo %cminf% Started build generation.
	cmake --build . --config Release -- /m
	
	if %errorlevel% neq 0 (
		echo %cmexc% Build failed because Compiler.exe did not get generated properly. Retrying clean rebuild
		goto cmake
	) else (
		echo %cmsuc% Build succeeded!
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
cmake -A x64 ..

:: Build the project
echo %cminf% Started build generation.
cmake --build . --target clean
cmake --build . --config Release -- /m
	
if %errorlevel% neq 0 (
	echo %cmexc% Build failed because Compiler.exe did not get generated properly.
) else (
	echo %cmsuc% Build succeeded!
)
	
pause
exit /b