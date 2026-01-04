cmake_minimum_required(VERSION 3.29.2)

set(PROGRAM_VERSION "LuaRuntime")
set(PROGRAM_VERSION_NUMBER 5.5.0.0)

project("LuaRuntime" VERSION ${PROGRAM_VERSION_NUMBER} LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# MSVC runtime consistency
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Platform Detection
if (WIN32)
    message(STATUS "[LuaRuntime] Platform = Windows")
else()
    message(FATAL_ERROR "[LuaRuntime] Unsupported platform. Only Windows is supported.")
endif()

# Build Type Detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(IS_RELEASE TRUE)
else()
    message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'!")
endif()

# Paths
set(LUA_DIR "${CMAKE_SOURCE_DIR}/src")

# Lua VM + standard library sources ONLY
set(LUA_SOURCES
    lapi.c
    lauxlib.c
    lbaselib.c
    lcode.c
    lcorolib.c
    lctype.c
    ldblib.c
    ldebug.c
    ldo.c
    ldump.c
    lfunc.c
    lgc.c
    linit.c
    liolib.c
    llex.c
    lmathlib.c
    lmem.c
    lobject.c
    lopcodes.c
    loslib.c
    lparser.c
    lstate.c
    lstring.c
    lstrlib.c
    ltable.c
    ltablib.c
    ltm.c
    lutf8lib.c
    lvm.c
    lzio.c
    loadlib.c
    lundump.c
)

list(TRANSFORM LUA_SOURCES PREPEND "${LUA_DIR}/")

# SHARED LIBRARY (lua.dll)
add_library(lua SHARED ${LUA_SOURCES})

# Output suffix handling
if (IS_RELEASE)
    set(LUA_SUFFIX "")
else()
    set(LUA_SUFFIX "d")
endif()

# latest lua is 5.5
set_target_properties(lua PROPERTIES
    OUTPUT_NAME "lua5_5${LUA_SUFFIX}"
)

# Includes
target_include_directories(lua PRIVATE
    "${LUA_DIR}"
)

# Preprocessor Defines
target_compile_definitions(lua PRIVATE
    LUA_BUILD_AS_DLL
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# Disable warnings Lua doesn't care about (optional but sane)
if (MSVC)
    target_compile_options(lua PRIVATE
        /wd4244  # conversion
        /wd4267  # size_t conversion
        /wd4996  # deprecated CRT
    )
endif()
