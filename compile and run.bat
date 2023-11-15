REM move to the build folder
cd "C:\Users\sande\Documents\cpp_projects\Eclipse engine\build"
REM configure project and generate build files
cmake -G "MinGW Makefiles" ..
REM generate the executable
mingw32-make
REM run the generated executable
.\CMakeTarget.exe