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

project("Game" VERSION ${PROJECT_VERSION})

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")
set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/icon.rc")

# Game files
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# External files
set(EXT_ASSIMP_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Assimp")
set(EXT_GLAD_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Glad")
set(EXT_GLFW_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/GLFW")
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/GLM")
set(EXT_IMGUI_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/ImGui")
set(EXT_MAGIC_ENUM_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Magic_enum")
set(EXT_STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Stb_image")
set(EXT_MATH_UTILS_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Math-utils")

# Add GLAD
add_library(glad STATIC ${EXT_GLAD_DIR}/glad.c)
target_include_directories(glad PUBLIC ${EXT_GLAD_DIR}/..)

# Add GLFW
set(GLFW_LIBRARY_PATH "${EXT_GLFW_DIR}/glfw3.lib")

# Add Assimp
set(ASSIMP_LIBRARY_PATH "${EXT_ASSIMP_DIR}/assimp-vc143-mt.lib")

# Add magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${EXT_MAGIC_ENUM_DIR})

# Add Math-utils
add_library(math_utils INTERFACE)
target_include_directories(math_utils INTERFACE ${EXT_MATH_UTILS_DIR})

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad magic_enum math_utils)

# Add your source files
file(GLOB_RECURSE SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${CMAKE_SOURCE_DIR}/src/*/*.cpp
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
add_executable(Game ${SOURCE_FILES} ${RESOURCE_FILE})

# Forces the name to be Game
set_target_properties(Game PROPERTIES OUTPUT_NAME "Game")

target_compile_features(Game PRIVATE cxx_std_20)

target_include_directories(Game PRIVATE 
	${INCLUDE_DIR}/core
	${INCLUDE_DIR}/file
	${INCLUDE_DIR}/graphics
	${INCLUDE_DIR}/graphics/gui
	${INCLUDE_DIR}/graphics/shapes
	${INCLUDE_DIR}/utils
	${EXT_ASSIMP_DIR}
	${EXT_GLAD_DIR}
	${EXT_GLFW_DIR}
	${EXT_GLM_DIR}
	${EXT_GLM_DIR}/detail
	${EXT_GLM_DIR}/ext
	${EXT_GLM_DIR}/gtc
	${EXT_GLM_DIR}/gtx
	${EXT_GLM_DIR}/simd
	${EXT_MAGIC_ENUM_DIR}
	${EXT_IMGUI_DIR}
	${EXT_STB_IMAGE_DIR}
	${EXT_MATH_UTILS_DIR}
)

target_link_libraries(Game PRIVATE external_libs glad ${GLFW_LIBRARY_PATH} ${ASSIMP_LIBRARY_PATH})

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Game PRIVATE GLFW_INCLUDE_NONE)

# Set the subsystem to WINDOWS
#set_target_properties(Game PROPERTIES
#    WIN32_EXECUTABLE TRUE
#)

# Set the entry point (MSVC-specific)
#if (MSVC)
#    set_target_properties(Game PROPERTIES
#       LINK_FLAGS "/ENTRY:mainCRTStartup"
#    )
#endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Game DESTINATION ${CMAKE_INSTALL_BINDIR})

# Create "project" directory in files folder if it doesnt exist
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/files/project)

# Copy "files" directory to the install directory after build
add_custom_command(TARGET Game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:Game>/files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/files 
	$<TARGET_FILE_DIR:Game>/files
)

# Copy assimp dll to the install directory after build
add_custom_command(TARGET Game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Game>/assimp-vc143-mt.dll
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/files/external\ dlls/assimp-vc143-mt.dll
    $<TARGET_FILE_DIR:Game>/assimp-vc143-mt.dll
)

# Copy "LICENSE.md" to install directory after build
add_custom_command(TARGET Game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Game>/LICENSE.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/LICENSE.md
    $<TARGET_FILE_DIR:Game>/LICENSE.md
)

# Include CPack to enable packaging
include(CPack)