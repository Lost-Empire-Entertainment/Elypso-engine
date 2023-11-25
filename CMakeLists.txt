cmake_minimum_required(VERSION 3.27)

set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 1)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION})

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# Add GLAD
add_library(glad STATIC ${SRC_DIR}/glad/glad.c)
target_include_directories(glad PUBLIC ${INCLUDE_DIR})

# Add GLFW
set(GLFW_LIBRARY_PATH "${LIB_DIR}/glfw/glfw3.lib")

add_executable(ElypsoEngine 
	${SRC_DIR}/main.cpp
    ${SRC_DIR}/glad/glad.c
)

target_include_directories(ElypsoEngine PRIVATE 
	${INCLUDE_DIR} 
	${INCLUDE_DIR}/glad
	${INCLUDE_DIR}/glfw
	${INCLUDE_DIR}/glm
)

target_link_libraries(ElypsoEngine PRIVATE glad ${GLFW_LIBRARY_PATH})

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS ElypsoEngine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Debugging messages for installation paths
message(STATUS "[Engine Debug] Cmake Install Prefix: ${CMAKE_INSTALL_PREFIX}")

# Debugging messages for additional information
message(STATUS "[Engine Debug] GLFW Library File: ${LIB_DIR}/glfw/glfw3.lib")
message(STATUS "[Engine Debug] OpenGL Include Directories: ${OPENGL_INCLUDE_DIRS}")

# Debugging messages for build type and compiler information
message(STATUS "[Engine Debug] CMake Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "[Engine Debug] C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[Engine Debug] C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# Debugging messages for project version
message(STATUS "[Engine Debug] Project Version: ${PROJECT_VERSION}")

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
