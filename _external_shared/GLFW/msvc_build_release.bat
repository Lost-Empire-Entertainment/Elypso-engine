@echo off

:: This batch file builds GLFW from source code using MSVC (cl.exe).
:: Place this script in the root GLFW folder and run it to build glfw3.lib.

:: Set the root folder as the location of this script
set "GLFW_ROOT=%~dp0"
set "BUILD_DIR=%GLFW_ROOT%msvc-build-release"
set "INSTALL_DIR=%GLFW_ROOT%msvc-install-release"

:: Ensure Visual Studio environment is set up
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" || (
    echo [ERROR] Failed to set up Visual Studio environment.
    exit /b 1
)

:: Create the build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%" || (
    echo [ERROR] Failed to access build directory.
    exit /b 1
)

:: Configure GLFW with CMake
cmake -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_COMPILER=cl ^
  -DCMAKE_CXX_COMPILER=cl ^
  -DBUILD_SHARED_LIBS=OFF ^
  -DGLFW_BUILD_EXAMPLES=OFF ^
  -DGLFW_BUILD_TESTS=OFF ^
  -DGLFW_BUILD_DOCS=OFF ^
  -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
  "%GLFW_ROOT%" || (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

:: Build GLFW with Ninja
ninja || (
    echo [ERROR] Build process failed.
    exit /b 1
)

:: Install GLFW
ninja install || (
    echo [ERROR] Install process failed.
    exit /b 1
)

:: Success message
echo [SUCCESS] GLFW built and installed successfully.
echo Static library: %INSTALL_DIR%\lib\glfw3.lib
echo Headers: %INSTALL_DIR%\include\GLFW

pause
exit /b
