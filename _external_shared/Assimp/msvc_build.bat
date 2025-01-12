@echo off

:: This batch file builds Assimp from source code as a DLL using MSVC (cl.exe).
:: Place this script in the root Assimp folder (same directory as CMakeLists.txt) and run it.

:: Paths and directories
set "ASSIMP_ROOT=%~dp0"
set "BUILD_DIR=%ASSIMP_ROOT%msvc-build"
set "INSTALL_DIR=%ASSIMP_ROOT%msvc-install"

:: Ensure Visual Studio environment is set up
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" || (
    echo [ERROR] Failed to set up Visual Studio environment.
    pause
    exit /b 1
)

:: Create the build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%" || (
    echo [ERROR] Failed to access build directory.
    pause
    exit /b 1
)

:: Configure Assimp with CMake
cmake -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_COMPILER=cl ^
  -DCMAKE_CXX_COMPILER=cl ^
  -DBUILD_SHARED_LIBS=ON ^
  -DASSIMP_BUILD_TESTS=OFF ^
  -DASSIMP_BUILD_SAMPLES=OFF ^
  -DASSIMP_BUILD_DOCS=OFF ^
  -DASSIMP_BUILD_ASSIMP_TOOLS=OFF ^
  -DASSIMP_NO_EXPORT=ON ^
  -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF ^
  -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF ^
  -DASSIMP_BUILD_OBJ_IMPORTER=ON ^
  -DASSIMP_BUILD_FBX_IMPORTER=ON ^
  -DASSIMP_BUILD_GLTF_IMPORTER=ON ^
  -DASSIMP_NO_LOGGING=ON ^
  -DASSIMP_ENABLE_DEBUGGING=OFF ^
  -DASSIMP_USE_ZLIB=ON ^
  -DCMAKE_C_FLAGS="/O2 /GL /DNDEBUG /EHsc" ^
  -DCMAKE_CXX_FLAGS="/O2 /GL /DNDEBUG /EHsc" ^
  -DCMAKE_EXE_LINKER_FLAGS="/LTCG /OPT:REF /OPT:ICF" ^
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
echo [SUCCESS] Assimp DLL built and installed successfully.
echo DLL: %INSTALL_DIR%\bin\assimp-vc143-mt.dll
echo Import library: %INSTALL_DIR%\lib\assimp-vc143-mt.lib
echo Headers: %INSTALL_DIR%\include

pause
exit /b
