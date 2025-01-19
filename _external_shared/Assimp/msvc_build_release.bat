@echo off

:: This batch file builds Assimp from source code using MSVC (cl.exe) for Release.
:: Paths and directories
set "ASSIMP_ROOT=%~dp0"
set "BUILD_DIR=%ASSIMP_ROOT%msvc-build-release"
set "INSTALL_DIR=%ASSIMP_ROOT%msvc-install-release"

:: Ensure Visual Studio environment is set up
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" || (
    echo [ERROR] Failed to set up Visual Studio environment.
    pause
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

:: Configure Assimp with CMake
echo [INFO] Configuring Assimp with CMake...
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
  -DASSIMP_BUILD_OBJ_IMPORTER=ON ^
  -DASSIMP_BUILD_FBX_IMPORTER=ON ^
  -DASSIMP_BUILD_GLTF_IMPORTER=ON ^
  -DASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=OFF ^
  -DCMAKE_C_FLAGS="/Os /DNDEBUG /EHsc /fp:fast /GS- /MD /WX /w44273 /w44251 /w44996 /Gw /Gy /Zc:inline /Zc:wchar_t-" ^
  -DCMAKE_CXX_FLAGS="/Os /DNDEBUG /EHsc /fp:fast /GS- /MD /WX /w44273 /w44251 /w44996 /Gw /Gy /Zc:inline /Zc:wchar_t-" ^
  -DCMAKE_EXE_LINKER_FLAGS="/OPT:REF /OPT:ICF /INCREMENTAL:NO /DEBUG:NONE /IGNORE:4099" ^
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF ^
  -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
  -DCMAKE_WARN_DEPRECATED=OFF -Wno-dev ^
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
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do set "TIME_END=%%a:%%b:%%c"

:: Success message
echo [SUCCESS] Assimp built and installed successfully.
echo ---------------------------------------------
echo Dynamic Link Library (DLL): %INSTALL_DIR%\bin\
echo Include headers: %INSTALL_DIR%\include
echo Build duration: %TIME_START% - %TIME_END%
echo ---------------------------------------------

pause
exit /b
