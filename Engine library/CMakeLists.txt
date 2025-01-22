cmake_minimum_required (VERSION 3.29.5)

if (WIN32)
	# Enable hot reload for MSVC
	if (POLICY CMP0141)
		cmake_policy(SET CMP0141 NEW)
		set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
	endif()
	# Explicitly set the MSVC toolset for Visual Studio
	if (MSVC)
		set(CMAKE_GENERATOR_TOOLSET "v143")
	endif()
endif()

# Specify the compiler for Linux builds
if (UNIX)
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Toggles engine mode for engine library
remove_definitions(-DENGINE_MODE)

# Toggles discord sdk mode for engine library
# REPLACE 'remove' WITH 'add' TO USE DISCORD GAME SDK
remove_definitions(-DDISCORD_MODE)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION})

get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")

# Engine files
set(ENGINE_SRC_DIR "${PARENT_DIR}/Engine/src/engine")
set(ENGINE_INCLUDE_DIR "${PARENT_DIR}/Engine/include/engine")

# Game files
set(GAME_SRC_DIR "${PARENT_DIR}/Engine/src/game")
set(GAME_INCLUDE_DIR "${PARENT_DIR}/Engine/include/game")

# External files
set(EXT_ASSIMP_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Assimp")
set(EXT_GLAD_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Glad")
set(EXT_GLFW_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/GLFW")
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/GLM")
set(EXT_IMGUI_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/ImGui")
set(EXT_MAGIC_ENUM_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/magic_enum")
set(EXT_STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Stb_image")
# UNCOMMENT TO USE DISCORD GAME SDK
# set(EXT_DISCORD_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Discord")

# Add GLAD
add_library(glad STATIC ${EXT_GLAD_DIR}/glad.c)
target_include_directories(glad PUBLIC ${EXT_GLAD_DIR}/..)

# Add GLFW
if (WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(GLFW_LIBRARY_PATH "${EXT_GLFW_DIR}/release/glfw3dll.lib")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(GLFW_LIBRARY_PATH "${EXT_GLFW_DIR}/debug/glfw3dll.lib")
	endif()
else()
	find_package(glfw3 REQUIRED)
endif()

# Add Assimp
if (WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(ASSIMP_LIBRARY_PATH "${EXT_ASSIMP_DIR}/release/assimp-vc143-mt.lib")
		set(ASSIMP_LIBRARY_PATH2 "${EXT_ASSIMP_DIR}/release/zlibstatic.lib")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(ASSIMP_LIBRARY_PATH "${EXT_ASSIMP_DIR}/debug/assimp-vc143-mtd.lib")
		set(ASSIMP_LIBRARY_PATH2 "${EXT_ASSIMP_DIR}/debug/zlibstaticd.lib")
	endif()
else()
	find_package(assimp REQUIRED)
endif()

# Add magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${EXT_MAGIC_ENUM_DIR})

# Add discord
# UNCOMMENT TO USE DISCORD GAME SDK
# set(DISCORD_LIBRARY_PATH "${EXT_DISCORD_DIR}/discord_game_sdk.dll.lib")

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
# UNCOMMENT TO USE DISCORD GAME SDK
# file(GLOB DISCORD_SRC_FILES ${EXT_DISCORD_DIR}/src/*.cpp)

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
	# UNCOMMENT TO USE DISCORD GAME SDK
	# ${DISCORD_SRC_FILES}
)

# Build shared (.dll or .so) and static (.lib or .a) libraries
add_library(Elypso_engine_shared SHARED ${SOURCE_FILES})
add_library(Elypso_engine_static STATIC ${SOURCE_FILES})

# Set the output names based on build type for clarity
set_target_properties(Elypso_engine_shared PROPERTIES 
    OUTPUT_NAME "Elypso_engine$<$<CONFIG:Debug>:D>"
)
set_target_properties(Elypso_engine_static PROPERTIES 
    OUTPUT_NAME "Elypso engine$<$<CONFIG:Debug>:D>"
)

# Common compile features for both shared and static libraries
foreach(TARGET_NAME Elypso_engine_shared Elypso_engine_static)
    target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

    target_include_directories(${TARGET_NAME} PRIVATE
        ${GAME_INCLUDE_DIR}/core
        ${ENGINE_INCLUDE_DIR}/core
        ${ENGINE_INCLUDE_DIR}/file
        ${ENGINE_INCLUDE_DIR}/graphics
        ${ENGINE_INCLUDE_DIR}/graphics/components
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
        # UNCOMMENT TO USE DISCORD GAME SDK
        # ${EXT_DISCORD_DIR}/include
    )

    # Define GLFW_INCLUDE_NONE for each build
    target_compile_definitions(${TARGET_NAME} PRIVATE GLFW_INCLUDE_NONE)
endforeach()

# Link the appropriate libraries for Windows and Linux
if (WIN32)
    target_link_libraries(Elypso_engine_shared PRIVATE external_libs ${GLFW_LIBRARY_PATH} ${ASSIMP_LIBRARY_PATH} ${ASSIMP_LIBRARY_PATH2})
    target_link_libraries(Elypso_engine_static PRIVATE external_libs ${GLFW_LIBRARY_PATH} ${ASSIMP_LIBRARY_PATH} ${ASSIMP_LIBRARY_PATH2})
else()
    target_link_libraries(Elypso_engine_shared PRIVATE external_libs glfw assimp ${X11_LIBRARIES})
    target_link_libraries(Elypso_engine_static PRIVATE external_libs glfw assimp ${X11_LIBRARIES})
endif()

# UNCOMMENT TO USE DISCORD GAME SDK
# target_link_libraries(Elypso_engine_shared PUBLIC ${DISCORD_LIBRARY_PATH})
# target_link_libraries(Elypso_engine_static PUBLIC ${DISCORD_LIBRARY_PATH})

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Elypso_engine_shared Elypso_engine_static DESTINATION ${CMAKE_INSTALL_BINDIR})

# Include CPack to enable packaging
include(CPack)
