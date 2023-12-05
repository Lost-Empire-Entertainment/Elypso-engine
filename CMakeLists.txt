cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 2)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION})

# Reusable message types printed to console
set(eninf "[ENGINE_INFO]")

# Paths to frequently used directories
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(FILES_DIR "${CMAKE_SOURCE_DIR}/files")

# Add GLAD
add_library(glad STATIC ${SRC_DIR}/external/glad/glad.c)
target_include_directories(glad PUBLIC ${INCLUDE_DIR})

# Add GLFW
set(GLFW_LIBRARY_PATH "${LIB_DIR}/glfw/glfw3.lib")

# Add magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${INCLUDE_DIR}/magic_enum)

# Add your source files
file(GLOB_RECURSE SOURCE_FILES
    ${SRC_DIR}/*.cpp
    ${SRC_DIR}/*.c
	${INCLUDE_DIR}/*.hpp
    ${INCLUDE_DIR}/*.h
)

# Add the executable
add_executable(Elypso_engine ${SOURCE_FILES})

target_compile_features(Elypso_engine PRIVATE cxx_std_20)

target_include_directories(Elypso_engine PRIVATE 
	${INCLUDE_DIR}/engine
	${INCLUDE_DIR}/engine/core
	${INCLUDE_DIR}/engine/graphics
	${INCLUDE_DIR}/engine/input
	${INCLUDE_DIR}/external
	${INCLUDE_DIR}/external/glad
	${INCLUDE_DIR}/external/glfw
	${INCLUDE_DIR}/external/glm
	${INCLUDE_DIR}/external/magic_enum
	${INCLUDE_DIR}/external/imgui
)

target_link_libraries(Elypso_engine PRIVATE glad ${GLFW_LIBRARY_PATH})

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Elypso_engine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Various debugging messages to help
message(STATUS "${eninf} Project Version: ${PROJECT_VERSION}")
message(STATUS "${eninf} C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "${eninf} C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "${eninf} CMake Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "${eninf} Cmake Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "${eninf} GLFW Library File: ${LIB_DIR}/glfw/glfw3.lib")

# Set the path to the license file
set(LICENSE_FILE ${CMAKE_SOURCE_DIR}/LICENSE.md)
# Specify the license file for CPack to include in the installer
set(CPACK_RESOURCE_FILE_LICENSE ${LICENSE_FILE})

# Set the output directory for CPack-generated files
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_SOURCE_DIR}/install")

# Enable NSIS debugging
set(CPACK_NSIS_DEBUG ON)

# Common package properties
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "Elypso engine")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# Set package properties for Windows (NSIS)
set(CPACK_NSIS_DISPLAY_NAME "Elypso engine")
set(CPACK_NSIS_PACKAGE_NAME "Elypso engine")
set(CPACK_PACKAGE_FILE_NAME "Elypso engine installer")
set(CPACK_NSIS_EXECUTABLE_NAME "Elypso engine")
set(CPACK_NSIS_CONTACT "Lost Empire Entertainment")

# Add uninstaller configuration for NSIS
set(CPACK_NSIS_CREATE_UNINSTALLED_TARGET TRUE)
set(CPACK_NSIS_MODIFY_PATH ON)

# Installer icon
set(CPACK_NSIS_MUI_ICON "${FILES_DIR}/icon.ico")
# Executable icon
set(CPACK_NSIS_INSTALLED_ICON_NAME "${FILES_DIR}/icon.ico")

# Include CPack to enable packaging
include(CPack)
