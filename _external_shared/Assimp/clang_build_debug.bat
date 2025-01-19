@echo off

:: This batch file builds Assimp from source code using Clang.
:: Place this script in the root Assimp folder and run it to build the static library.

:: Paths and directories
set "ASSIMP_ROOT=%~dp0"
set "BUILD_DIR=%ASSIMP_ROOT%clang-build-debug"
set "INSTALL_DIR=%ASSIMP_ROOT%clang-install-debug"
set PATH=C:\BuildTools\mingw-bin\bin;C:\BuildTools\cmake-bin\bin;C:\BuildTools\ninja-bin;%PATH%

:: Create the build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%" || (
    echo [ERROR] Failed to access build directory.
    pause
    exit /b 1
)

:: Configure Assimp with CMake
cmake -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Debug ^
  -DCMAKE_C_COMPILER=clang ^
  -DCMAKE_CXX_COMPILER=clang++ ^
  -DBUILD_SHARED_LIBS=OFF ^
  -DASSIMP_BUILD_TESTS=OFF ^
  -DASSIMP_BUILD_SAMPLES=OFF ^
  -DASSIMP_BUILD_DOCS=OFF ^
  -DASSIMP_NO_EXPORT=ON ^
  -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF ^
  -DASSIMP_BUILD_OBJ_IMPORTER=ON ^
  -DASSIMP_BUILD_FBX_IMPORTER=ON ^
  -DASSIMP_BUILD_GLTF_IMPORTER=ON ^
  -DASSIMP_NO_LOGGING=OFF ^
  -DASSIMP_ENABLE_DEBUGGING=ON ^
  -DASSIMP_BUILD_ASSIMP_TOOLS=OFF ^
  -DASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1 ^
  -DCMAKE_CXX_FLAGS="-Wno-unknown-pragmas -Wno-error=unknown-pragmas" ^
  -DCMAKE_EXE_LINKER_FLAGS="-Wl,--gc-sections" ^
  -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
  "%ASSIMP_ROOT%" || (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

:: Build Assimp with Ninja
ninja || (
    echo [ERROR] Build process failed.
    pause
    exit /b 1
)

:: Install Assimp
ninja install || (
    echo [ERROR] Install process failed.
    pause
    exit /b 1
)

:: Success message
echo [SUCCESS] Assimp built and installed successfully.
echo Static library: %INSTALL_DIR%\lib\libassimp.a
echo Headers: %INSTALL_DIR%\include

pause
exit /b
