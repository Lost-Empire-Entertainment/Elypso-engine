@echo off

:: This batch file builds Assimp from source code using Clang (Debug mode).

:: Paths and directories
set "ASSIMP_ROOT=%~dp0"
set "BUILD_DIR=%ASSIMP_ROOT%clang-build-debug"
set "INSTALL_DIR=%ASSIMP_ROOT%clang-install-debug"

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

:: Configure Assimp with CMake
echo [INFO] Configuring Assimp with CMake...
cmake -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Debug ^
  -DCMAKE_C_COMPILER=clang ^
  -DCMAKE_CXX_COMPILER=clang++ ^
  -DBUILD_SHARED_LIBS=ON ^
  -DASSIMP_BUILD_TESTS=OFF ^
  -DASSIMP_BUILD_SAMPLES=OFF ^
  -DASSIMP_BUILD_DOCS=OFF ^
  -DASSIMP_NO_EXPORT=ON ^
  -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF ^
  -DASSIMP_BUILD_OBJ_IMPORTER=ON ^
  -DASSIMP_BUILD_FBX_IMPORTER=ON ^
  -DASSIMP_BUILD_GLTF_IMPORTER=ON ^
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
echo [INFO] Building Assimp...
ninja -j%NUMBER_OF_PROCESSORS% || (
    echo [ERROR] Build process failed.
    pause
    exit /b 1
)

:: Install Assimp
echo [INFO] Installing Assimp...
ninja install || (
    echo [ERROR] Install process failed.
    pause
    exit /b 1
)

:: Record end time
set TIME_END=%TIME%

:: Success message
echo [SUCCESS] Assimp built and installed successfully.
echo ---------------------------------------------
echo Dynamic Link Library (DLL): %INSTALL_DIR%\bin\
echo Include headers: %INSTALL_DIR%\include
echo Build duration: %TIME_START% - %TIME_END%
echo ---------------------------------------------

pause
exit /b
