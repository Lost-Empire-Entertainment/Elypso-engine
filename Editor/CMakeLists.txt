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
    message(STATUS "[EDITOR] Platform = Windows")
elseif(UNIX)
	message(STATUS "[EDITOR] Platform = Unix")
else()
    message(FATAL_ERROR "[EDITOR] Unsupported platform. Only Windows and Unix are supported.")
endif()

# Build Type Detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(IS_RELEASE TRUE)
else()
    message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
endif()

# Paths
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
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
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*/*.cpp"
)

# Executable
add_executable(ElypsoEditor ${SOURCE_FILES})

# Enable C++ exception handling semantics for MSVC
if (MSVC)
    target_compile_options(ElypsoEditor PRIVATE /EHsc)
endif()
	
set_target_properties(ElypsoEditor PROPERTIES OUTPUT_NAME "Elypso engine")
target_compile_features(ElypsoEditor PRIVATE cxx_std_20)

# Includes
file(GLOB_RECURSE HEADERS
	configure_depends
	"${CMAKE_SOURCE_DIR}/include/*.hpp"
)
target_sources(ElypsoEditor PRIVATE ${HEADERS})
target_include_directories(ElypsoEditor PRIVATE
	"${INCLUDE_DIR}"
	"${EXT_SHARED_DIR}"
	"${EXT_SHARED_DIR}/KalaWindow/include"
	"${EXT_SHARED_DIR}/KalaUI/include"
	"${EXT_SHARED_DIR}/KalaLua/include"
)

# Preprocessor Defines
target_compile_definitions(ElypsoEditor PRIVATE
	WIN32_LEAN_AND_MEAN # cleaner windows.h
	NOMINMAX            # skips min/max windows macros in favor of the std variants
	UNICODE             # selects wide api variants over ansi for UTF16 windows functions
	_UNICODE            # uses wide text types in the C runtime layer
)

# Link libraries
target_link_libraries(ElypsoEditor PRIVATE
	opengl32
	${WINDOW_LIBRARY_PATH}
	${UI_LIBRARY_PATH}
	${LUA_LIBRARY_PATH}
)

# Hide console in release mode
# if(IS_RELEASE)
#     set_target_properties(ElypsoEditor PROPERTIES WIN32_EXECUTABLE TRUE)
#     if (MSVC)
#         set_target_properties(ElypsoEditor PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
#     endif()
# endif()

# Set Windows Details tab data
# set(ICON_DIR "${CMAKE_SOURCE_DIR}/files/icon.ico")

# Expose version string
add_compile_definitions(
	PROGRAM_VERSION="${PROGRAM_VERSION}"
)

# Installation
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS ElypsoEditor DESTINATION ${CMAKE_INSTALL_BINDIR})

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

set(BIN_TARGET_DIR "$<TARGET_FILE_DIR:ElypsoEditor>")

foreach(BIN_FILE ${BIN_FILES})
    get_filename_component(BIN_NAME ${BIN_FILE} NAME)
    add_custom_command(TARGET ElypsoEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
			"${BIN_FILE}"
			"${BIN_TARGET_DIR}/${BIN_NAME}"
    )
endforeach()

# Copy important files
set(COPY_TARGETS README.md LICENSE.md)
foreach(FILE ${COPY_TARGETS})
add_custom_command(TARGET ElypsoEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f 
		"$<TARGET_FILE_DIR:ElypsoEditor>/${FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_SOURCE_DIR}/${FILE}"
		"$<TARGET_FILE_DIR:ElypsoEditor>/${FILE}"
)
endforeach()

# Copy entire docs folder
add_custom_command(TARGET ElypsoEditor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E rm -rf
		"$<TARGET_FILE_DIR:ElypsoEditor>/docs"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/docs"
		"$<TARGET_FILE_DIR:ElypsoEditor>/docs"
)

# Package
include(CPack)
