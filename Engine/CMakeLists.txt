cmake_minimum_required(VERSION 3.29.2)

set(PROGRAM_VERSION "Elypso engine 2026.1.0")
set(PROGRAM_VERSION_NUMBER 2026.1.0.0)

project("Elypso engine" VERSION ${PROGRAM_VERSION_NUMBER} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Runtime lib handling for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Platform Detection
if (WIN32)
    message(STATUS "[ELYPSO_ENGINE] Platform = Windows")
elseif(UNIX)
	message(STATUS "[ELYPSO_ENGINE] Platform = Unix")
else()
    message(FATAL_ERROR "[ELYPSO_ENGINE] Unsupported platform. Only Windows and Unix are supported.")
endif()

# Build Type Detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(IS_RELEASE TRUE)
else()
    message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
endif()

# Directory Setup
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set(EXT_SHARED_DIR "${CMAKE_SOURCE_DIR}/../_external_shared")
set(EXT_WINDOW_DIR "${EXT_SHARED_DIR}/Kalawindow")
set(EXT_UI_DIR "${EXT_SHARED_DIR}/KalaUI")
set(EXT_PHYSICS_DIR "${EXT_SHARED_DIR}/KalaPhysics")
set(EXT_AUDIO_DIR "${EXT_SHARED_DIR}/KalaAudio")
set(EXT_LUA_DIR "${EXT_SHARED_DIR}/KalaLua")

# Library Paths
if(IS_RELEASE)
	set(WINDOW_LIBRARY_PATH "${EXT_WINDOW_DIR}/release/KalaWindow1.lib")
	set(UI_LIBRARY_PATH "${EXT_UI_DIR}/release/KalaUI1.lib")
	set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/release/KalaPhysics1.lib")
	set(AUDIO_LIBRARY_PATH "${EXT_AUDIO_DIR}/release/KalaAudio1.lib")
    set(LUA_LIBRARY_PATH "${EXT_LUA_DIR}/release/KalaLua1.lib")
else()
	set(WINDOW_LIBRARY_PATH "${EXT_WINDOW_DIR}/debug/KalaWindow1d.lib")
	set(UI_LIBRARY_PATH "${EXT_UI_DIR}/debug/KalaUI1d.lib")
	set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/debug/KalaPhysics1d.lib")
	set(AUDIO_LIBRARY_PATH "${EXT_AUDIO_DIR}/debug/KalaAudio1d.lib")
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
	"${EXT_SHARED_DIR}/KalaPhysics/include"
	"${EXT_SHARED_DIR}/KalaAudio/include"
	"${EXT_SHARED_DIR}/KalaLua/include"
)

# Preprocessor Defines
target_compile_definitions(ElypsoEngine PRIVATE 
	opengl32
	LIB_EXPORT          # dll import/export
	WIN32_LEAN_AND_MEAN # cleaner windows.h
	NOMINMAX            # skips min/max windows macros in favor of the std variants
	UNICODE             # selects wide api variants over ansi for UTF16 windows functions
	_UNICODE            # uses wide text types in the C runtime layer
)

# Link libraries
target_link_libraries(ElypsoEngine PRIVATE
	opengl32
	${WINDOW_LIBRARY_PATH}
	${UI_LIBRARY_PATH}
	${PHYSICS_LIBRARY_PATH}
	${AUDIO_LIBRARY_PATH}
	${LUA_LIBRARY_PATH}
)

# Expose version string
add_compile_definitions(
	PROGRAM_VERSION="${PROGRAM_VERSION}"
)

# Installation
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS ElypsoEngine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy files directory
add_custom_command(TARGET ElypsoEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory 
		"$<TARGET_FILE_DIR:ElypsoEngine>/files"
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
		"${CMAKE_SOURCE_DIR}/../files"
		"$<TARGET_FILE_DIR:ElypsoEngine>/files"
)

# Copy external binaries
if(IS_RELEASE)
	set(BIN_KALAWINDOW "${EXT_WINDOW_DIR}/release")
	set(BIN_KALAUI "${EXT_UI_DIR}/release")
	set(BIN_KALAPHYSICS "${EXT_PHYSICS_DIR}/release")
	set(BIN_KALAAUDIO "${EXT_AUDIO_DIR}/release")
	set(BIN_KALALUA "${EXT_LUA_DIR}/release")
else()
	set(BIN_KALAWINDOW "${EXT_WINDOW_DIR}/debug")
	set(BIN_KALAUI "${EXT_UI_DIR}/debug")
	set(BIN_KALAPHYSICS "${EXT_PHYSICS_DIR}/debug")
	set(BIN_KALAAUDIO "${EXT_AUDIO_DIR}/debug")
	set(BIN_KALALUA "${EXT_LUA_DIR}/debug")
endif()

if (IS_RELEASE)
	file(GLOB BIN_FILES
		"${BIN_KALAWINDOW}/*.dll"
		"${BIN_KALAUI}/*.dll"
		"${BIN_KALAPHYSICS}/*.dll"
		"${BIN_KALAAUDIO}/*.dll"
		"${BIN_KALALUA}/*.dll"
	)
else()
	file(GLOB BIN_FILES
		"${BIN_KALAWINDOW}/*.dll"
		"${BIN_KALAWINDOW}/*.pdb"
		"${BIN_KALAUI}/*.dll"
		"${BIN_KALAUI}/*.pdb"
		"${BIN_KALAPHYSICS}/*.dll"
		"${BIN_KALAPHYSICS}/*.pdb"
		"${BIN_KALAAUDIO}/*.dll"
		"${BIN_KALAAUDIO}/*.pdb"
		"${BIN_KALALUA}/*.dll"
		"${BIN_KALALUA}/*.pdb"
	)
endif()

set(RUNTIME_DIR "${CMAKE_BINARY_DIR}/runtime")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${RUNTIME_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${RUNTIME_DIR}")
set(BIN_TARGET_DIR "${RUNTIME_DIR}")

foreach(BIN_FILE ${BIN_FILES})
    if(EXISTS "${BIN_FILE}")
        file(COPY
            "${BIN_FILE}"
            DESTINATION "${BIN_TARGET_DIR}"
        )
    endif()
endforeach()

# Copy important files
set(COPY_TARGETS
    README.md
    LICENSE.md
    logo.png
)

foreach(FILE ${COPY_TARGETS})
    if(EXISTS "${CMAKE_SOURCE_DIR}/${FILE}")
        file(COPY
            "${CMAKE_SOURCE_DIR}/${FILE}"
            DESTINATION "${BIN_TARGET_DIR}"
        )
    endif()
endforeach()

set(RESOURCE_DIRS docs files)

foreach(DIR ${RESOURCE_DIRS})
    if(EXISTS "${CMAKE_SOURCE_DIR}/${DIR}")
        file(REMOVE_RECURSE "${BIN_TARGET_DIR}/${DIR}")
        file(COPY
            "${CMAKE_SOURCE_DIR}/${DIR}"
            DESTINATION "${BIN_TARGET_DIR}"
        )
    endif()
endforeach()

if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        DESTINATION "${CMAKE_SOURCE_DIR}"
    )
endif()
