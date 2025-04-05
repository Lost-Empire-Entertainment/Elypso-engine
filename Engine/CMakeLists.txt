cmake_minimum_required(VERSION 3.30.3)

# Specify the compiler for Linux builds
if (UNIX)
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Toggles engine mode for engine
add_definitions(-DENGINE_MODE)

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

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")

# Specify resource file for Windows
if (WIN32)
    set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/icon.rc")
endif()

# Engine files
set(ENGINE_SRC_DIR "${CMAKE_SOURCE_DIR}/src/engine")
set(ENGINE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/engine")

# Game files
set(GAME_SRC_DIR "${CMAKE_SOURCE_DIR}/src/game")
set(GAME_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/game")

# External files
set(EXT_GLFW_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/GLFW")
set(EXT_ASSIMP_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Assimp")
set(EXT_GLAD_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Glad")
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/GLM")
set(EXT_IMGUI_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/ImGui")
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
		set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/release/libKalaPhysics.so")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/debug/libKalaPhysicsD.so")
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
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/release/libKalaCrashHandler.so")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CRASH_LIBRARY_PATH "${EXT_CRASH_DIR}/debug/libKalaCrashHandlerD.so")
	endif()
endif()

# Add magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${EXT_MAGIC_ENUM_DIR})

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad magic_enum)

# Add your source files
file(GLOB_RECURSE ENGINE_SOURCE_FILES CONFIGURE_DEPENDS
	${ENGINE_SRC_DIR}/*.cpp
	${ENGINE_SRC_DIR}/*/*.cpp
)
file(GLOB_RECURSE GAME_SOURCE_FILES CONFIGURE_DEPENDS
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
	${EXT_STB_IMAGE_DIR}/stb_image.cpp
)

# Build as engine executable
add_executable(Elypso_engine ${SOURCE_FILES})

if (WIN32)
	set_source_files_properties(${RESOURCE_FILE} PROPERTIES LANGUAGE RC)
	target_sources(Elypso_engine PRIVATE ${RESOURCE_FILE})
endif()
	
set_target_properties(Elypso_engine PROPERTIES OUTPUT_NAME "Elypso engine")

target_compile_features(Elypso_engine PRIVATE cxx_std_20)

target_include_directories(Elypso_engine PRIVATE 
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
    ${EXT_GLFW_DIR}
    ${EXT_GLAD_DIR}
    ${EXT_GLM_DIR}
	${EXT_GLM_DIR}/detail
	${EXT_GLM_DIR}/ext
	${EXT_GLM_DIR}/gtc
	${EXT_GLM_DIR}/gtx
	${EXT_GLM_DIR}/simd
    ${EXT_MAGIC_ENUM_DIR}
    ${EXT_IMGUI_DIR}
    ${EXT_STB_IMAGE_DIR}
	${EXT_MINIAUDIO_DIR}
	${EXT_PHYSICS_DIR}
	${EXT_CRASH_DIR}
)

# Apply all directories to target
target_include_directories(Elypso_engine PRIVATE ${ENGINE_INCLUDE_DIRS})

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

# Disable console on release
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	# Set the subsystem to WINDOWS
	set_target_properties(Elypso_engine PROPERTIES
		WIN32_EXECUTABLE TRUE
	)

	# Set the entry point (MSVC-specific)
	if (MSVC)
		set_target_properties(Elypso_engine PROPERTIES
		LINK_FLAGS "/ENTRY:mainCRTStartup"
		)
	endif()
endif()

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

# Copy all DLLs to the install directory after build
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	set(DLL_ORIGIN_DIR "${CMAKE_SOURCE_DIR}/files/external dlls/release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(DLL_ORIGIN_DIR "${CMAKE_SOURCE_DIR}/files/external dlls/debug")
endif()
set(DLL_TARGET_DIR "$<TARGET_FILE_DIR:Elypso_engine>")

file(GLOB DLL_FILES "${DLL_ORIGIN_DIR}/*")
	
foreach(DLL_FILE ${DLL_FILES})
    get_filename_component(DLL_NAME ${DLL_FILE} NAME)
    add_custom_command(TARGET Elypso_engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
            ${DLL_FILE} 
            ${DLL_TARGET_DIR}/${DLL_NAME}
    )
endforeach()

set(COPY_TARGETS LICENSE.md README.md SECURITY.md LIBRARIES.md)
foreach(FILE ${COPY_TARGETS})
  add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
	$<TARGET_FILE_DIR:Elypso_engine>/${FILE}
	COMMAND ${CMAKE_COMMAND} -E copy
	${PARENT_DIR_PATH}/${FILE}
	$<TARGET_FILE_DIR:Elypso_engine>/${FILE}
  )
endforeach()

# Include CPack to enable packaging
include(CPack)
