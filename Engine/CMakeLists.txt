cmake_minimum_required(VERSION 3.29.2)

set(PROGRAM_VERSION "Elypso engine 2026.1.0")
set(PROGRAM_VERSION_NUMBER 1.0.0.0)

project("Elypso engine" VERSION ${PROGRAM_VERSION_NUMBER} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform Detection
if (WIN32)
    message(STATUS "[ELYPSO_ENGINE] Platform = Windows")
else()
    message(FATAL_ERROR "[ELYPSO_ENGINE] Unsupported platform. Only Windows is supported.")
endif()

# Build Type Detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(IS_RELEASE TRUE)
else()
    message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
endif()

# Runtime lib handling for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Directory Setup
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set(EXT_SHARED_DIR "${CMAKE_SOURCE_DIR}/../_external_shared")
set(EXT_WINDOW_DIR "${EXT_SHARED_DIR}/Kalawindow")
set(EXT_UI_DIR "${EXT_SHARED_DIR}/KalaUI")
set(EXT_LUA_DIR "${EXT_SHARED_DIR}/KalaLua")

# Library Paths
if(IS_RELEASE)
	set(WINDOW_LIBRARY_PATH "${EXT_WINDOW_DIR}/release/KalaWindow1.lib")
	set(UI_LIBRARY_PATH "${EXT_UI_DIR}/release/KalaUI1.lib")
    set(LUA_LIBRARY_PATH "${EXT_LUA_DIR}/release/KalaLua1.lib")
else()
	set(WINDOW_LIBRARY_PATH "${EXT_WINDOW_DIR}/debug/KalaWindow1d.lib")
	set(UI_LIBRARY_PATH "${EXT_UI_DIR}/debug/KalaUI1d.lib")
    set(LUA_LIBRARY_PATH "${EXT_LUA_DIR}/debug/KalaLua1d.lib")
endif()

# Source Files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*/*.cpp"
)

# SHARED LIBRARY (.dll)
add_library(ElypsoEngine SHARED ${SOURCE_FILES})

# Enable exception handling for MSVC
if (MSVC)
    target_compile_options(ElypsoEngine PRIVATE /EHsc)
endif()

# Get suffix
if (IS_RELEASE)
	set(KALA_SUFFIX "")
else()
	set(KALA_SUFFIX "d")
endif()

# Set output name with renderer suffix and debug postfix
set_target_properties(ElypsoEngine PROPERTIES
	OUTPUT_NAME "ElypsoEngine1${KALA_SUFFIX}"
)

# Includes
file(GLOB_RECURSE HEADERS
	configure_depends
	"${CMAKE_SOURCE_DIR}/include/*.hpp"
)
target_sources(ElypsoEngine PRIVATE ${HEADERS})
target_include_directories(ElypsoEngine PRIVATE
	"${INCLUDE_DIR}"
	"${EXT_SHARED_DIR}"
	"${EXT_SHARED_DIR}/KalaWindow/include"
	"${EXT_SHARED_DIR}/KalaUI/include"
	"${EXT_SHARED_DIR}/KalaLua/include"
)

# Preprocessor Defines
target_compile_definitions(ElypsoEngine PRIVATE 
	LIB_EXPORT
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# Link libraries
target_link_libraries(ElypsoEngine PRIVATE
	opengl32
	${WINDOW_LIBRARY_PATH}
	${UI_LIBRARY_PATH}
	${LUA_LIBRARY_PATH}
)

# Hide console in release mode
# if(IS_RELEASE)
#     set_target_properties(ElypsoEngine PROPERTIES WIN32_EXECUTABLE TRUE)
#     if (MSVC)
#         set_target_properties(ElypsoEngine PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
#     endif()
# endif()

# Expose version string
add_compile_definitions(
	PROGRAM_VERSION="${PROGRAM_VERSION}"
)

# Installation
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS ElypsoEngine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy files directory
add_custom_command(TARGET ElypsoEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:ElypsoEngine>/files"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/files" "$<TARGET_FILE_DIR:ElypsoEngine>/files"
)

# Copy external binaries
if(IS_RELEASE)
	set(BIN_WINDOW "${EXT_WINDOW_DIR}/release")
	set(BIN_UI "${EXT_UI_DIR}/release")
	set(BIN_LUA "${EXT_LUA_DIR}/release")
else()
	set(BIN_WINDOW "${EXT_WINDOW_DIR}/debug")
	set(BIN_UI "${EXT_UI_DIR}/debug")
	set(BIN_LUA "${EXT_LUA_DIR}/debug")
endif()

if (IS_RELEASE)
	file(GLOB BIN_FILES
		"${BIN_WINDOW}/*.dll"
		"${BIN_UI}/*.dll"
		"${BIN_LUA}/*.dll"
	)
else()
	file(GLOB BIN_FILES
		"${BIN_WINDOW}/*.dll"
		"${BIN_WINDOW}/*.pdb"
		"${BIN_UI}/*.dll"
		"${BIN_UI}/*.pdb"
		"${BIN_LUA}/*.dll"
		"${BIN_LUA}/*.pdb"
	)
endif()

set(BIN_TARGET_DIR "$<TARGET_FILE_DIR:ElypsoEngine>")

foreach(BIN_FILE ${BIN_FILES})
    get_filename_component(BIN_NAME ${BIN_FILE} NAME)
    add_custom_command(TARGET ElypsoEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
			"${BIN_FILE}"
			"${BIN_TARGET_DIR}/${BIN_NAME}"
    )
endforeach()
