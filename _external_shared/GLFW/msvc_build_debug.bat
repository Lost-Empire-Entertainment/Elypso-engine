@echo off

:: This batch file builds GLFW from source code using MSVC (cl.exe) for Debug.

:: Set the root folder as the location of this script
set "GLFW_ROOT=%~dp0"
set "BUILD_DIR=%GLFW_ROOT%msvc-build-debug"
set "INSTALL_DIR=%GLFW_ROOT%msvc-install-debug"

:: Ensure Visual Studio environment is set up
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
    exit /b 1
)

:: Configure GLFW with CMake
echo [INFO] Configuring GLFW with CMake...
cmake -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Debug ^
  -DCMAKE_C_COMPILER=cl ^
  -DBUILD_SHARED_LIBS=ON ^
  -DGLFW_BUILD_EXAMPLES=OFF ^
  -DGLFW_BUILD_TESTS=OFF ^
  -DGLFW_BUILD_DOCS=OFF ^
  -DCMAKE_C_FLAGS="/Od /Zi /EHsc /MDd" ^
  -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
  "%GLFW_ROOT%" || (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

:: Build GLFW with Ninja
echo [INFO] Building GLFW...
ninja -j%NUMBER_OF_PROCESSORS% || (
    echo [ERROR] Build process failed.
    exit /b 1
)

:: Install GLFW
echo [INFO] Installing GLFW...
ninja install || (
    echo [ERROR] Install process failed.
    exit /b 1
)

:: Record end time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_END=%%a:%%b:%%c"

:: Success message
echo [SUCCESS] GLFW built and installed successfully.
echo ---------------------------------------------
echo Static library: %INSTALL_DIR%\lib\glfw3.lib
echo Include headers: %INSTALL_DIR%\include\GLFW
echo Build duration: %TIME_START% - %TIME_END%
echo ---------------------------------------------

pause
exit /b 0
