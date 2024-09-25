cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Turns engine mode off for engine library
remove_definitions(-DENGINE_MODE)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION})

get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")

# Used for copying "Elypso engine.lib" from its folder to Game folder
set(LIB_ORIGIN "${CMAKE_SOURCE_DIR}/build/Release/Elypso engine.lib")
set(LIB_TARGET "${CMAKE_SOURCE_DIR}/../Game/Elypso engine.lib")

# Engine files
set(ENGINE_SRC_DIR "${PARENT_DIR}/Engine/src/engine")
set(ENGINE_INCLUDE_DIR "${PARENT_DIR}/Engine/include/engine")

# Game files
set(GAME_SRC_DIR "${PARENT_DIR}/Engine/src/game")
set(GAME_INCLUDE_DIR "${PARENT_DIR}/Engine/include/game")

# External files
set(EXT_ASSIMP_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Assimp")
set(EXT_GLAD_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Glad")
set(EXT_GLFW_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/GLFW")
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/GLM")
set(EXT_IMGUI_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/ImGui")
set(EXT_MAGIC_ENUM_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Magic_enum")
set(EXT_STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Stb_image")

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

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad magic_enum)

# Add your source files
file(GLOB_RECURSE ENGINE_SOURCE_FILES
	${ENGINE_SRC_DIR}/*.cpp
	${ENGINE_SRC_DIR}/*/*.cpp
)
file(GLOB_RECURSE GAME_SOURCE_FILES
	${GAME_SRC_DIR}/*.cpp
	${GAME_SRC_DIR}/*/*.cpp
)
set(SOURCE_FILES
	${ENGINE_SOURCE_FILES}
	${GAME_SOURCE_FILES}
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
	${EXT_IMGUI_DIR}/imgui_demo.cpp
	${EXT_STB_IMAGE_DIR}/stb_image.cpp
)

# Build as static library
add_library(Elypso_engine STATIC ${SOURCE_FILES})
set_target_properties(Elypso_engine PROPERTIES OUTPUT_NAME "Elypso engine")

target_compile_features(Elypso_engine PRIVATE cxx_std_20)

target_include_directories(Elypso_engine
	PUBLIC
		${GAME_INCLUDE_DIR}/core
	PRIVATE
		${ENGINE_INCLUDE_DIR}/core
		${ENGINE_INCLUDE_DIR}/file
		${ENGINE_INCLUDE_DIR}/graphics
		${ENGINE_INCLUDE_DIR}/graphics/gui
		${ENGINE_INCLUDE_DIR}/graphics/shapes
		${ENGINE_INCLUDE_DIR}/physics
		${ENGINE_INCLUDE_DIR}/utils
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
)

target_link_libraries(Elypso_engine PRIVATE external_libs glad ${GLFW_LIBRARY_PATH} ${ASSIMP_LIBRARY_PATH})

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Elypso_engine PRIVATE GLFW_INCLUDE_NONE)

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Elypso_engine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy "Elypso engine.lib" to Game folder
add_custom_command(TARGET Elypso_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    ${LIB_TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy
	${LIB_ORIGIN}
    ${LIB_TARGET}
)

# Include CPack to enable packaging
include(CPack)