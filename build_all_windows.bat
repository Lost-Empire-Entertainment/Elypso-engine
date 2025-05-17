@echo off

set "PROJECT_ROOT=%~dp0"

set CLEAN_ARG=
if "%1" == "clean" set CLEAN_ARG=clean

echo =====================================
echo [INFO] Copying binaries...
echo =====================================
echo.

cd "%PROJECT_ROOT%\_external_shared"
cmd /c "copy_windows.bat"
if errorlevel 1 (
    echo [ERROR] Failed to copy binaries.
    pause
    exit /b 1
)
cd ..

echo =====================================
echo [INFO] Building Elypso engine in Release mode...
echo =====================================
echo.

:: Then build engine
cd "%PROJECT_ROOT%\Engine"
cmd /c "build_windows_release.bat" %CLEAN_ARG%
if errorlevel 1 (
    echo [ERROR] Release build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [INFO] Building Elypso engine in Debug mode...
echo =====================================
echo.

cmd /c "build_windows_debug.bat" %CLEAN_ARG%
if errorlevel 1 (
    echo [ERROR] Debug build failed.
    pause
    exit /b 1
)

echo =====================================
echo [INFO] Building Elypso engine library in Release mode...
echo =====================================
echo.

:: Then build engine library
cd "%PROJECT_ROOT%\Engine library" %CLEAN_ARG%
cmd /c "build_windows_release.bat"
if errorlevel 1 (
    echo [ERROR] Release build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [INFO] Building Elypso engine library in Debug mode...
echo =====================================
echo.

cmd /c "build_windows_debug.bat" %CLEAN_ARG%
if errorlevel 1 (
    echo [ERROR] Debug build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [SUCCESS] Finished building and installing Elypso engine and Elypso engine library!
echo =====================================
echo.

pause
exit /b 0
