cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Turns engine mode on for engine
add_definitions(-DENGINE_MODE)
# remove_definitions(-DENGINE_MODE)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION})

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")
set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/icon.rc")

# Engine files
set(ENGINE_SRC_DIR "${CMAKE_SOURCE_DIR}/src/engine")
set(ENGINE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/engine")

# Game files
set(GAME_SRC_DIR "${CMAKE_SOURCE_DIR}/src/game")
set(GAME_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/game")

# External files
set(EXT_ASSIMP_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Assimp")
set(EXT_GLAD_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Glad")
set(EXT_GLFW_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/GLFW")
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/GLM")
set(EXT_IMGUI_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/ImGui")
set(EXT_MAGIC_ENUM_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Magic_enum")
set(EXT_STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Stb_image")
set(EXT_DISCORD_DIR "${CMAKE_SOURCE_DIR}../../_external_shared/Discord")

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

# Add discord
set(DISCORD_LIBRARY_PATH "${EXT_DISCORD_DIR}/discord_game_sdk.dll.lib")

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

# Add discord source files
file(GLOB DISCORD_SRC_FILES ${EXT_DISCORD_DIR}/src/*.cpp)

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
	${DISCORD_SRC_FILES}
)

# Build as engine exe
add_executable(Elypso_engine ${SOURCE_FILES} ${RESOURCE_FILE})
set_target_properties(Elypso_engine PROPERTIES OUTPUT_NAME "Elypso engine")

target_compile_features(Elypso_engine PRIVATE cxx_std_20)

target_include_directories(Elypso_engine PRIVATE 
	${GAME_INCLUDE_DIR}/core
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
	${EXT_DISCORD_DIR}/include
)

target_link_libraries(Elypso_engine PRIVATE external_libs glad ${GLFW_LIBRARY_PATH} ${ASSIMP_LIBRARY_PATH})
target_link_libraries(Elypso_engine PUBLIC ${DISCORD_LIBRARY_PATH})

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Elypso_engine PRIVATE GLFW_INCLUDE_NONE)

# Set the subsystem to WINDOWS
#set_target_properties(Elypso_engine PROPERTIES
#    WIN32_EXECUTABLE TRUE
#)

# Set the entry point (MSVC-specific)
#if (MSVC)
#    set_target_properties(Elypso_engine PROPERTIES
#       LINK_FLAGS "/ENTRY:mainCRTStartup"
#    )
#endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Elypso_engine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy "files" directory to the install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
	$<TARGET_FILE_DIR:Elypso_engine>/files
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/files 
	$<TARGET_FILE_DIR:Elypso_engine>/files
)

# Copy assimp dll to the install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
	$<TARGET_FILE_DIR:Elypso_engine>/assimp-vc143-mt.dll
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_SOURCE_DIR}/files/external\ dlls/assimp-vc143-mt.dll
	$<TARGET_FILE_DIR:Elypso_engine>/assimp-vc143-mt.dll
)

# Copy discord dll to the install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
	$<TARGET_FILE_DIR:Elypso_engine>/discord_game_sdk.dll
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_SOURCE_DIR}/files/external\ dlls/discord_game_sdk.dll
	$<TARGET_FILE_DIR:Elypso_engine>/discord_game_sdk.dll
)

# Copy "LICENSE.md" to install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
	$<TARGET_FILE_DIR:Elypso_engine>/LICENSE.md
	COMMAND ${CMAKE_COMMAND} -E copy
	${PARENT_DIR_PATH}/LICENSE.md
	$<TARGET_FILE_DIR:Elypso_engine>/LICENSE.md
)

# Copy "README.md" to install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
	$<TARGET_FILE_DIR:Elypso_engine>/README.md
	COMMAND ${CMAKE_COMMAND} -E copy
	${PARENT_DIR_PATH}/README.md
	$<TARGET_FILE_DIR:Elypso_engine>/README.md
)

# Copy "SECURITY.md" to install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
	$<TARGET_FILE_DIR:Elypso_engine>/SECURITY.md
	COMMAND ${CMAKE_COMMAND} -E copy
	${PARENT_DIR_PATH}/SECURITY.md
	$<TARGET_FILE_DIR:Elypso_engine>/SECURITY.md
)

# Copy "LIBRARIES.md" to install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
	$<TARGET_FILE_DIR:Elypso_engine>/LIBRARIES.md
	COMMAND ${CMAKE_COMMAND} -E copy
	${PARENT_DIR_PATH}/LIBRARIES.md
	$<TARGET_FILE_DIR:Elypso_engine>/LIBRARIES.md
)

# Include CPack to enable packaging
include(CPack)