@echo off
REM Get the path where the batch file is run from
set "current_path=%~dp0"

REM Move to the build folder relative to the batch file path
cd /d "%current_path%build"

REM Configure project and generate build files
cmake -G "MinGW Makefiles" ..

REM Generate the executable
mingw32-make

REM Run the generated executable
.\CMakeTarget.exe