@echo off

:: This batch file builds ElypsoPhysics from source code using MSVC (cl.exe) for Release.

:: Set the root folder as the location of this script
set "PHYSICS_ROOT=%~dp0"
set "BUILD_DIR=%PHYSICS_ROOT%build-release"
set "INSTALL_DIR=%PHYSICS_ROOT%install-release"

:: Ensure Visual Studio environment is set up correctly
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" || (
    echo [ERROR] Failed to set up Visual Studio environment.
    exit /b 1
)

:: Record start time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_START=%%a:%%b:%%c"

:: Create the build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%" || (
    echo [ERROR] Failed to access build directory: %BUILD_DIR%
	pause
    exit /b 1
)

:: Configure ElypsoPhysics with CMake
echo [INFO] Configuring ElypsoPhysics with CMake...
cmake -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_COMPILER=cl ^
  -DBUILD_SHARED_LIBS=ON ^
  -DCMAKE_C_FLAGS="/O2 /GL /DNDEBUG /EHsc /MD" ^
  -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
  -Wno-dev ^
  "%PHYSICS_ROOT%" || (
    echo [ERROR] CMake configuration failed.
	pause
    exit /b 1
)

:: Build ElypsoPhysics with Ninja
echo [INFO] Building ElypsoPhysics...
ninja -j%NUMBER_OF_PROCESSORS% || (
    echo [ERROR] Build process failed.
	pause
    exit /b 1
)

:: Install ElypsoPhysics
echo [INFO] Installing ElypsoPhysics...
ninja install || (
    echo [ERROR] Install process failed.
	pause
    exit /b 1
)

:: Record end time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_END=%%a:%%b:%%c"

:: Success message
echo [SUCCESS] ElypsoPhysics built and installed successfully.
echo ---------------------------------------------
echo Shared library: %INSTALL_DIR%\bin\ElypsoPhysics.dll
echo Static library: %INSTALL_DIR%\lib\ElypsoPhysics_Static.lib
echo Include headers: %INSTALL_DIR%\include
echo Build duration: %TIME_START% - %TIME_END%
echo ---------------------------------------------

pause
exit /b 0
