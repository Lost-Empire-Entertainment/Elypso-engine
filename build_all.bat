@echo off
setlocal

set "root_path=%~dp0"
echo Root path: %root_path%

set "engine_bat_path=%root_path%Engine\build_windows.bat"
echo Engine bat path: %engine_bat_path%

set "library_bat_path=%root_path%Engine library\build_windows.bat"
echo Engine library bat path: %library_bat_path%

if not exist "%engine_bat_path%" (
	echo Failed to find engine bat file at "%engine_bat_path%"!
	pause
	exit /b 1
)
if not exist "%library_bat_path%" (
	echo Failed to find engine library bat file at "%library_bat_path%"!
	pause
	exit /b 1
)

echo.
echo ==============================
echo Building Elypso engine in debug mode...
echo ==============================
echo.

call "%engine_bat_path%" cmake debug

echo.
echo ==============================
echo Building Elypso engine in release mode...
echo ==============================
echo.

call "%engine_bat_path%" cmake release

echo.
echo ==============================
echo Building Elypso engine library in debug mode...
echo ==============================
echo.

call "%library_bat_path%" cmake debug

echo.
echo ==============================
echo Building Elypso engine library in release mode...
echo ==============================
echo.

call "%library_bat_path%" cmake release

echo.
echo ==============================
echo Finished building Elypso engine and Elypso engine library!
echo ==============================
echo.

pause
endlocal
exit /b 0