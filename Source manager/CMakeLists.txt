# Copyright (C) 2023 Greenlaser
# This program comes with ABSOLUTELY NO WARRANTY.
# This is free software, and you are welcome to redistribute it under certain conditions.
# Read LICENSE.md and EULA.md for more information.

cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project ("Source manager" VERSION ${PROJECT_VERSION})

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/src/external")

# Add GLFW
set(GLFW_LIBRARY_PATH "${LIB_DIR}/glfw/glfw3.lib")

# Add your source files
set(SOURCE_FILES
	${SRC_DIR}/program/main.cpp
)

# Add your external files
list(APPEND SOURCE_FILES
	${EXTERNAL_DIR}/imgui/imgui.cpp
	${EXTERNAL_DIR}/imgui/imgui_demo.cpp
	${EXTERNAL_DIR}/imgui/imgui_draw.cpp
	${EXTERNAL_DIR}/imgui/imgui_impl_glfw.cpp
	${EXTERNAL_DIR}/imgui/imgui_impl_opengl3.cpp
	${EXTERNAL_DIR}/imgui/imgui_tables.cpp
	${EXTERNAL_DIR}/imgui/imgui_widgets.cpp
)

# Add the executable
add_executable (Source_manager ${SOURCE_FILES})

target_include_directories(Source_manager PRIVATE 
	${INCLUDE_DIR}/program
	${INCLUDE_DIR}/external/glfw
	${INCLUDE_DIR}/external/imgui
)

target_link_libraries(Source_manager PRIVATE ${GLFW_LIBRARY_PATH})

# Set the subsystem to WINDOWS
# set_target_properties(Elypso_engine PROPERTIES
#     WIN32_EXECUTABLE TRUE
# )

# Set the entry point (MSVC-specific)
# if (MSVC)
#     set_target_properties(Elypso_engine PROPERTIES
#        LINK_FLAGS "/ENTRY:mainCRTStartup"
#     )
# endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Source_manager DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy "EULA.md", "LICENSE.md" and "README.md" to install directory after build
add_custom_command(TARGET Source_manager POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Source_manager>/EULA.md
    COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/EULA.md
    $<TARGET_FILE_DIR:Source_manager>/EULA.md
	
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Source_manager>/LICENSE.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/LICENSE.md
    $<TARGET_FILE_DIR:Source_manager>/LICENSE.md
	
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Source_manager>/README.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/README.md
    $<TARGET_FILE_DIR:Source_manager>/README.md
)

# Include CPack to enable packaging
include(CPack)