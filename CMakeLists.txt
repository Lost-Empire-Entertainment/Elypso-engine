cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "include/*.h")

# Set the installation folder name
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "Installation directory")

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

# Add the path to the include, lib, and source directories
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

set(SOURCE_FILES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/glad/glad.c
)

add_executable(ElypsoEngine ${SOURCE_FILES})

# Set up GLFW
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
  IMPORTED_LOCATION "${LIB_DIR}/glfw3.lib"
)

# Set up GLAD
add_library(glad STATIC ${SRC_DIR}/glad/glad.c)
target_include_directories(glad PUBLIC ${INCLUDE_DIR}/glad)
set_target_properties(glad PROPERTIES 
	LINKER_LANGUAGE CXX
)

find_package(OpenGL REQUIRED)
target_link_libraries(ElypsoEngine PRIVATE glfw OpenGL::GL glad)

target_include_directories(ElypsoEngine PRIVATE 
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/glfw
    ${INCLUDE_DIR}/glm
    ${INCLUDE_DIR}/glad
	${OPENGL_INCLUDE_DIRS}
)

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS ElypsoEngine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Debugging messages for installation paths
message(STATUS "[Engine Debug] Cmake Install Prefix: ${CMAKE_INSTALL_PREFIX}")

# Debugging messages for additional information
message(STATUS "[Engine Debug] GLFW Library File: ${LIB_DIR}/glfw3.lib")
message(STATUS "[Engine Debug] OpenGL Include Directories: ${OPENGL_INCLUDE_DIRS}")

# Debugging messages for the list of source files
message(STATUS "[Engine Debug] List of Source Files:")
foreach(SOURCE_FILE ${SOURCE_FILES})
    message(STATUS "    ${SOURCE_FILE}")
endforeach()

# Debugging messages for build type and compiler information
message(STATUS "[Engine Debug] CMake Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "[Engine Debug] C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[Engine Debug] C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# Debugging messages for project version
message(STATUS "[Engine Debug] Project Version: ${PROJECT_VERSION}")

# Install the target
install(TARGETS ElypsoEngine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Include CPack to enable packaging
include(CPack)

# Common package properties
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "ElypsoEngine")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# Set package properties for Windows (NSIS)
set(CPACK_NSIS_DISPLAY_NAME "Elypso Engine")
set(CPACK_NSIS_PACKAGE_NAME "ElypsoEngine")
set(CPACK_NSIS_EXECUTABLE_NAME "ElypsoEngine")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
set(CPACK_NSIS_CONTACT "Lost Empire Entertainment")

# Add uninstaller configuration for NSIS
set(CPACK_NSIS_CREATE_UNINSTALLED_TARGET TRUE)
set(CPACK_NSIS_MODIFY_PATH ON)

# Installer icon
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/files/icon.ico")
# Uninstaller icon
set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_CURRENT_SOURCE_DIR}/files/icon.ico")
