# Check and set the default build type if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("Elypso engine")

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Add source to this project's executable.
add_executable(CMakeTarget "src/Main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# Include NSIS as the generator for CPack
set(CPACK_GENERATOR "NSIS")

# Set CPack NSIS variables (customize as needed)
set(CPACK_NSIS_PACKAGE_NAME "ElypsoEngine")
set(CPACK_NSIS_DISPLAY_NAME "Elypso Engine")
set(CPACK_NSIS_EXECUTABLE_NAME "ElypsoEngineSetup")
set(CPACK_NSIS_INSTALL_ROOT "C:/Program Files/ElypsoEngine")

# Include CPack to enable packaging
include(CPack)