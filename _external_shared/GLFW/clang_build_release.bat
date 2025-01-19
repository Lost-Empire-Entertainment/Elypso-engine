@echo off

:: This batch file builds GLFW from source code using Clang (Release mode).

:: Set the root folder as the location of this script
set "GLFW_ROOT=%~dp0"
set "BUILD_DIR=%GLFW_ROOT%clang-build-release"
set "INSTALL_DIR=%GLFW_ROOT%clang-install-release"

:: Add necessary tools to PATH
set PATH=C:\BuildTools\mingw-bin\bin;C:\BuildTools\cmake-bin\bin;C:\BuildTools\ninja-bin;%PATH%

:: Record start time
set TIME_START=%TIME%

:: Create the build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%" || (
    echo [ERROR] Failed to access build directory: %BUILD_DIR%
    pause
    exit /b 1
)

:: Configure GLFW with CMake
echo [INFO] Configuring GLFW with CMake...
cmake -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_COMPILER=clang ^
  -DBUILD_SHARED_LIBS=ON ^
  -DGLFW_BUILD_EXAMPLES=OFF ^
  -DGLFW_BUILD_TESTS=OFF ^
  -DGLFW_BUILD_DOCS=OFF ^
  -DCMAKE_C_FLAGS="-O3 -march=native -ffast-math -fno-lto -fdata-sections -ffunction-sections" ^
  -DCMAKE_EXE_LINKER_FLAGS="-Wl,--gc-sections -Wl,--strip-all" ^
  -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
  "%GLFW_ROOT%" || (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

:: Build GLFW with Ninja
echo [INFO] Building GLFW...
ninja -j%NUMBER_OF_PROCESSORS% || (
    echo [ERROR] Build process failed.
    pause
    exit /b 1
)

:: Install GLFW
echo [INFO] Installing GLFW...
ninja install || (
    echo [ERROR] Install process failed.
    pause
    exit /b 1
)

:: Record end time
set TIME_END=%TIME%

:: Success message
echo [SUCCESS] GLFW built and installed successfully.
echo ---------------------------------------------
echo Static library: %INSTALL_DIR%\lib\libglfw3.a
echo Include headers: %INSTALL_DIR%\include\GLFW
echo Build duration: %TIME_START% - %TIME_END%
echo ---------------------------------------------

pause
exit /b
