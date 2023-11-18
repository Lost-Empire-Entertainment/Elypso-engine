# Check and set the default build type if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "" FORCE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
   cmake_policy(SET CMP0141 NEW)
   set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 1)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION})

# Add source to this project's executable.
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.h")

add_executable(CMakeTarget ${SOURCE_FILES})

install(TARGETS CMakeTarget DESTINATION ${CMAKE_INSTALL_BINDIR})

# Debugging messages for installation paths
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")
message(STATUS "CMAKE_INSTALL_SRCDIR: ${CMAKE_INSTALL_SRCDIR}")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# Include directories for headers
target_include_directories(CMakeTarget PRIVATE "include")

# Include CPack to enable packaging
include(CPack)

# Common package properties
set(CPACK_PACKAGE_NAME "ElypsoEngine")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# Set package properties for Windows (NSIS)
set(CPACK_GENERATOR NSIS)
set(CPACK_NSIS_DISPLAY_NAME "Elypso Engine")
set(CPACK_NSIS_PACKAGE_NAME "ElypsoEngine")
set(CPACK_NSIS_EXECUTABLE_NAME "ElypsoEngine")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")

# Set package properties for Linux (DEB)
set(CPACK_GENERATOR DEB)

# Set package properties for macOS (DMG)
set(CPACK_GENERATOR DragNDrop)