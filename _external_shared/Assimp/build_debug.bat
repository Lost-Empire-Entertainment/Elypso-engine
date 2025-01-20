@echo off

:: This batch file builds Assimp from source code using MSVC (cl.exe) for Debug.

:: Paths and directories
set "ASSIMP_ROOT=%~dp0"
set "BUILD_DIR=%ASSIMP_ROOT%build-debug"
set "INSTALL_DIR=%ASSIMP_ROOT%install-debug"

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
    echo [ERROR] Failed to access build directory.
    pause
    exit /b 1
)

:: Configure Assimp with CMake
echo [INFO] Configuring Assimp with CMake...
cmake -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Debug ^
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
  -DCMAKE_C_FLAGS="/Od /GL /DNDEBUG /EHsc" ^
  -DCMAKE_CXX_FLAGS="/Od /GL /DNDEBUG /EHsc" ^
  -DCMAKE_EXE_LINKER_FLAGS="/LTCG /OPT:REF /OPT:ICF" ^
  -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
  -DCMAKE_SUPPRESS_DEVELOPER_WARNINGS=YES ^
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
