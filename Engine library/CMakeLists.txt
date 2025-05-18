cmake_minimum_required(VERSION 3.30.3)

# Specify the compiler for Linux builds
if (UNIX)
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Toggles engine mode for engine library
remove_definitions(-DENGINE_MODE)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION})

if (MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
endif()

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
set(EXT_IMGUIZMO_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/ImGuizmo")
set(EXT_MAGIC_ENUM_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/magic_enum")
set(EXT_STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Stb_image")
set(EXT_MINIAUDIO_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/miniaudio")
set(EXT_PHYSICS_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/KalaPhysics")
set(EXT_CRASH_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/KalaCrashHandler")

# Add GLAD
add_library(glad STATIC ${EXT_GLAD_DIR}/glad.c)
target_include_directories(glad PUBLIC ${EXT_GLAD_DIR}/..)

# Add GLFW
if (WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(GLFW_LIBRARY_PATH "${EXT_GLFW_DIR}/release/glfw3dll.lib")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(GLFW_LIBRARY_PATH "${EXT_GLFW_DIR}/debug/glfw3dll.lib")
	endif()
else()
	find_package(glfw3 REQUIRED)
endif()

# Add Assimp
if (WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(ASSIMP_LIBRARY_PATH "${EXT_ASSIMP_DIR}/release/assimp-vc143-mt.lib")
		set(ASSIMP_LIBRARY_PATH2 "${EXT_ASSIMP_DIR}/release/zlibstatic.lib")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(ASSIMP_LIBRARY_PATH "${EXT_ASSIMP_DIR}/debug/assimp-vc143-mtd.lib")
		set(ASSIMP_LIBRARY_PATH2 "${EXT_ASSIMP_DIR}/debug/zlibstaticd.lib")
	endif()
else()
	find_package(assimp REQUIRED)
endif()

# Add KalaPhysics
if (WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/release/KalaPhysics.lib")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/debug/KalaPhysicsD.lib")
	endif()
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/release/libKalaPhysics.a")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/debug/libKalaPhysicsD.a")
	endif()
endif()

# Add KalaCrashHandler
if (WIN32)
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/release/KalaCrashHandler.lib")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/debug/KalaCrashHandlerD.lib")
	endif()
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/release/libKalaCrashHandler.a")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/debug/libKalaCrashHandlerD.a")
	endif()
endif()

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
	${EXT_STB_IMAGE_DIR}/stb_image.cpp
	${EXT_IMGUI_DIR}/imgui.cpp
	${EXT_IMGUI_DIR}/imgui_draw.cpp
	${EXT_IMGUI_DIR}/imgui_impl_glfw.cpp
	${EXT_IMGUI_DIR}/imgui_impl_opengl3.cpp
	${EXT_IMGUI_DIR}/imgui_tables.cpp
	${EXT_IMGUI_DIR}/imgui_widgets.cpp
	${EXT_IMGUIZMO_DIR}/ImGuizmo.cpp
)

# Build as static library
add_library(Elypso_engine STATIC ${SOURCE_FILES})

# Set the library output name based on the build type
set_target_properties(Elypso_engine PROPERTIES 
    OUTPUT_NAME "Elypso engine$<$<CONFIG:Debug>:D>"
)

target_compile_features(Elypso_engine PRIVATE cxx_std_20)

target_include_directories(Elypso_engine PRIVATE
		# Engine include folders
		${GAME_INCLUDE_DIR}
		${ENGINE_INCLUDE_DIR}/core
		${ENGINE_INCLUDE_DIR}/file
		${ENGINE_INCLUDE_DIR}/graphics
		${ENGINE_INCLUDE_DIR}/graphics/components
		${ENGINE_INCLUDE_DIR}/graphics/gui
		${ENGINE_INCLUDE_DIR}/graphics/shapes
		${ENGINE_INCLUDE_DIR}/physics
		${ENGINE_INCLUDE_DIR}/utils
		
		# External include folders
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
		${EXT_IMGUIZMO_DIR}
		${EXT_STB_IMAGE_DIR}
		${EXT_MINIAUDIO_DIR}
		${EXT_PHYSICS_DIR}
		${EXT_CRASH_DIR}
)

if (WIN32)
	target_link_libraries(Elypso_engine PRIVATE 
		external_libs 
		${GLFW_LIBRARY_PATH} 
		${ASSIMP_LIBRARY_PATH} 
		${ASSIMP_LIBRARY_PATH2} 
		${PHYSICS_LIBRARY_PATH}
		${CRASH_LIBRARY_PATH})
else()
	find_package(Wayland QUIET)
	find_package(X11 QUIET)
	if (Wayland_FOUND)
		target_link_libraries(Elypso_engine PRIVATE 
			external_libs 
			glfw 
			assimp 
			Wayland::Client 
			${PHYSICS_LIBRARY_PATH}
			${CRASH_LIBRARY_PATH})
	elseif (X11_FOUND)
		target_link_libraries(Elypso_engine PRIVATE 
			external_libs 
			glfw 
			assimp 
			${X11_LIBRARIES} 
			${PHYSICS_LIBRARY_PATH}
			${CRASH_LIBRARY_PATH})
	endif()
endif()

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Elypso_engine PRIVATE GLFW_INCLUDE_NONE)

# Include CPack to enable packaging
include(CPack)
