@echo off

set "PROJECT_ROOT=%~dp0"

echo =====================================
echo [INFO] Copying external dlls...
echo =====================================
echo.

cd "%PROJECT_ROOT%\_external_shared"
cmd /c "copy_windows.bat"
if errorlevel 1 (
    echo [ERROR] DLL copy failed.
    pause
    exit /b 1
)
cd ..

echo =====================================
echo [INFO] Building engine in Release mode...
echo =====================================
echo.

:: Then build engine
cd "%PROJECT_ROOT%\Engine"
cmd /c "build_windows_release.bat"
if errorlevel 1 (
    echo [ERROR] Release build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [INFO] Building engine in Debug mode...
echo =====================================
echo.

cmd /c "build_windows_debug.bat"
if errorlevel 1 (
    echo [ERROR] Debug build failed.
    pause
    exit /b 1
)

echo =====================================
echo [INFO] Building engine library in Release mode...
echo =====================================
echo.

:: Then build engine library
cd "%PROJECT_ROOT%\Engine library"
cmd /c "build_windows_release.bat"
if errorlevel 1 (
    echo [ERROR] Release build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [INFO] Building engine library in Debug mode...
echo =====================================
echo.

cmd /c "build_windows_debug.bat"
if errorlevel 1 (
    echo [ERROR] Debug build failed.
    pause
    exit /b 1
)

echo.
echo =====================================
echo [SUCCESS] Finished building and installing engine and engine library!
echo =====================================
echo.

pause
exit /b 0
