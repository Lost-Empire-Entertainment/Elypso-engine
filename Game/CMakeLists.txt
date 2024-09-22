cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project("Game test")

# Set parent path
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")

# Set src and include file paths
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set(ENGINE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../Engine/include/engine")
set(GAME_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../Engine/include/game")

# Add your source files
file(GLOB_RECURSE SOURCE_FILES
	${SRC_DIR}/*.cpp
	${SRC_DIR}/*/*.cpp
)

add_executable(GameTest ${SOURCE_FILES})
set_target_properties(GameTest PROPERTIES OUTPUT_NAME "Game test")
target_compile_features(GameTest PRIVATE cxx_std_20)

target_include_directories(GameTest PRIVATE
	# Code added by user
	${INCLUDE_DIR}
	
	# Engine code for game
	${GAME_INCLUDE_DIR}/core
	
	# Engine code
	${ENGINE_INCLUDE_DIR}/core
	${ENGINE_INCLUDE_DIR}/file
	${ENGINE_INCLUDE_DIR}/graphics
	${ENGINE_INCLUDE_DIR}/graphics/gui
	${ENGINE_INCLUDE_DIR}/graphics/shapes
	${ENGINE_INCLUDE_DIR}/physics
	${ENGINE_INCLUDE_DIR}/utils
)

target_link_libraries(GameTest PRIVATE "${CMAKE_SOURCE_DIR}/Elypso engine.lib")

set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS GameTest DESTINATION ${CMAKE_INSTALL_BINDIR})

add_custom_command(TARGET GameTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove -f 
		"$<TARGET_FILE_DIR:GameTest>/Elypso engine.lib"
    COMMAND ${CMAKE_COMMAND} -E copy 
		"${CMAKE_SOURCE_DIR}/Elypso engine.lib" 
		"$<TARGET_FILE_DIR:GameTest>/Elypso engine.lib"
)

# Include CPack to enable packaging
include(CPack)