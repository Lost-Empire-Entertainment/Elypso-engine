cmake_minimum_required (VERSION 3.29)

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

project("Compiler" VERSION ${PROJECT_VERSION})

# Paths to frequently used directories
set(PARENT_DIR_PATH "${CMAKE_SOURCE_DIR}../../")
set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/icon.rc")

# Internal include files
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# External files
set(EXT_GLAD_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Glad")
set(EXT_GLFW_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/GLFW")
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/GLM")
set(EXT_IMGUI_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/ImGui")
set(EXT_STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Stb_image")

# Add GLAD
add_library(glad STATIC ${EXT_GLAD_DIR}/glad.c)
target_include_directories(glad PUBLIC ${EXT_GLAD_DIR}/..)

# Add GLFW
set(GLFW_LIBRARY_PATH "${EXT_GLFW_DIR}/glfw3.lib")

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad)

# Add your source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
	${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Add external source files
list(APPEND SOURCE_FILES
	${EXT_GLAD_DIR}/glad.c
	${EXT_IMGUI_DIR}/imgui.cpp
	${EXT_IMGUI_DIR}/imgui_draw.cpp
	${EXT_IMGUI_DIR}/imgui_impl_glfw.cpp
	${EXT_IMGUI_DIR}/imgui_impl_opengl3.cpp
	${EXT_IMGUI_DIR}/imgui_tables.cpp
	${EXT_IMGUI_DIR}/imgui_widgets.cpp
	${EXT_STB_IMAGE_DIR}/stb_image.cpp
)

# Add the executable
add_executable(Compiler ${SOURCE_FILES} ${RESOURCE_FILE})

# Forces output name
set_target_properties(Compiler PROPERTIES OUTPUT_NAME "Compiler")

target_compile_features(Compiler PRIVATE cxx_std_20)

target_include_directories(Compiler PRIVATE 
	${INCLUDE_DIR}
	${EXT_GLAD_DIR}
	${EXT_GLFW_DIR}
	${EXT_GLM_DIR}
	${EXT_GLM_DIR}
	${EXT_IMGUI_DIR}
	${EXT_STB_IMAGE_DIR}
)

target_link_libraries(Compiler PRIVATE external_libs ${GLFW_LIBRARY_PATH})

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Compiler PRIVATE GLFW_INCLUDE_NONE)

# Set the subsystem to WINDOWS
#set_target_properties(Compiler PROPERTIES
#    WIN32_EXECUTABLE TRUE
#)

# Set the entry point (MSVC-specific)
#if (MSVC)
#    set_target_properties(Compiler PROPERTIES
#       LINK_FLAGS "/ENTRY:mainCRTStartup"
#    )
#endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Compiler DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy "files" directory to the install directory after build
add_custom_command(TARGET Compiler POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:Compiler>/files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/files 
	$<TARGET_FILE_DIR:Compiler>/files
)

# Copy "LICENSE.md" to install directory after build
add_custom_command(TARGET Compiler POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Compiler>/LICENSE.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/LICENSE.md
    $<TARGET_FILE_DIR:Compiler>/LICENSE.md
)

# Copy "README.md" to install directory after build
add_custom_command(TARGET Compiler POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Compiler>/README.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/README.md
    $<TARGET_FILE_DIR:Compiler>/README.md
)

# Copy "LIBRARIES.md" to install directory after build
add_custom_command(TARGET Compiler POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Compiler>/LIBRARIES.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/LIBRARIES.md
    $<TARGET_FILE_DIR:Compiler>/LIBRARIES.md
)

# Include CPack to enable packaging
include(CPack)