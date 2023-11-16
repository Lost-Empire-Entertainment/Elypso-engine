# CMake project for Eclipse engine

cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Use generator expression for MinGW
project ("Eclipse_engine" LANGUAGES CXX)

# Add source to the project's executable.
add_executable (CMakeTarget "src/Main.cpp")

# Specify the C++ standard directly
set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 17)

# Include directories where header files are located
target_include_directories(CMakeTarget PRIVATE ${CMAKE_SOURCE_DIR}/src, ${CMAKE_SOURCE_DIR}/include)

# Set the path to the SFML root directory
set(CMAKE_PREFIX_PATH "C:/SFML-2.6.1/lib/cmake/SFML/")

# Find the SFML components
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# Link SFML libraries to the project
target_link_libraries(CMakeTarget PRIVATE sfml-graphics sfml-window sfml-system)

# Specify which version of C++ to compile with
set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 17)
set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD_REQUIRED ON)