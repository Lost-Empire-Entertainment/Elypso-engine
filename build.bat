REM Configure the project (Release build)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

REM Build the project
cmake --build build --config Release

REM Install the project
cmake --install build --config Release

REM Additional logging for CMake configuration and CPack
cmake . > cmake_log.txt 2>&1
cpack > cpack_log.txt 2>&1

REM Pause to keep the console window open for review
pause