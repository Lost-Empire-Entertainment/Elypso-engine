@echo off
:: Batch script to configure, build, and install Elypso engine

:: Change to the script directory
cd /d "%~dp0"

:: Configure the project (Release build) and generate NSIS installer
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR=NSIS

:: Build the project
cmake --build build --config Release

:: Package the project using CPack
cmake --install build --config Release
cd build
cpack
cd ..

:: Additional logging for CMake configuration and CPack
cmake . > cmake_log.txt 2>&1
cpack > cpack_log.txt 2>&1

:: Clean up unnecessary files and directories
del /q *.vcxproj
del /q *.filters
del /q *.sln
del /q *.cmake
del /q CMakeCache.txt
del /q cmake_log.txt
del /q cpack_log.txt
del /q install_manifest.txt
rmdir /s /q CMakeFiles
rmdir /s /q _CPACK_Packages

pause
