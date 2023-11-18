cmake_minimum_required(VERSION 3.27)

# Check and set the default build type if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "" FORCE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
   cmake_policy(SET CMP0141 NEW)
   set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 1)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# Add source to this project's executable.
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.h")

# Group source files in the IDE
source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

add_executable(CMakeTarget ${SOURCE_FILES})

# Set C++ standard to C++20 (if CMake version is greater than 3.27)
if (CMAKE_VERSION VERSION_GREATER 3.27)
  target_compile_features(CMakeTarget PRIVATE cxx_std_20)
endif()

set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS CMakeTarget DESTINATION ${CMAKE_INSTALL_BINDIR})

# Debugging messages for installation paths
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

# Include directories for headers
target_include_directories(CMakeTarget PRIVATE "include")

# Include CPack to enable packaging
include(CPack)

# Common package properties
set(CPACK_PACKAGE_NAME "ElypsoEngine")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# ---WINDOWS INSTALLER---

# Set package properties for Windows (NSIS)
set(CPACK_GENERATOR NSIS)
set(CPACK_NSIS_DISPLAY_NAME "Elypso Engine")
set(CPACK_NSIS_PACKAGE_NAME "ElypsoEngine")
set(CPACK_NSIS_EXECUTABLE_NAME "ElypsoEngine")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")

# Add uninstaller configuration for NSIS
set(CPACK_NSIS_CREATE_UNINSTALLED_TARGET TRUE)
set(CPACK_NSIS_MODIFY_PATH ON)

# Installer icon
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/files/icon.ico")
# Uninstaller icon
set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_CURRENT_SOURCE_DIR}/files/icon.ico")

# ---LINUX (.deb) INSTALLER---

# Set package properties for Linux (DEB)
set(CPACK_GENERATOR DEB)

# ---MAC (.dmg) INSTALLER---

# Set package properties for macOS (DMG)
set(CPACK_GENERATOR DragNDrop)