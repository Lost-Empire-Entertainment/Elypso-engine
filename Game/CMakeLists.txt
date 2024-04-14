cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Game" VERSION ${PROJECT_VERSION})

# Reusable message types printed to console

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")
set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/game/icon.rc")

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# Manually added source file paths
set(SRC_CORE_DIR "${CMAKE_SOURCE_DIR}/src/game/core")
set(SRC_FILE_DIR "${CMAKE_SOURCE_DIR}/src/game/file")
set(SRC_GRAPHICS_DIR "${CMAKE_SOURCE_DIR}/src/game/graphics")
set(SRC_GRAPHICS_SHAPE_DIR "${CMAKE_SOURCE_DIR}/src/game/graphics/shapes")
set(SRC_PHYSICS_DIR "${CMAKE_SOURCE_DIR}/src/game/physics")
set(SRC_UTILS_DIR "${CMAKE_SOURCE_DIR}/src/game/utils")
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/src/external")

# Add GLAD
add_library(glad STATIC ${SRC_DIR}/external/glad/glad.c)
target_include_directories(glad PUBLIC ${INCLUDE_DIR})

# Add GLFW
set(GLFW_LIBRARY_PATH "${LIB_DIR}/glfw/glfw3.lib")

# Add Assimp
set(ASSIMP_LIBRARY_PATH "${LIB_DIR}/assimp/assimp-vc143-mt.lib")

# Add magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${INCLUDE_DIR}/magic_enum)

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad magic_enum)

# Add your source files
set(SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/game/main.cpp
	${SRC_CORE_DIR}/core.cpp
	${SRC_CORE_DIR}/input.cpp
	${SRC_CORE_DIR}/timeManager.cpp
	${SRC_FILE_DIR}/configFile.cpp
	${SRC_FILE_DIR}/fileexplorer.cpp
	${SRC_FILE_DIR}/sceneFile.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/billboard.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/gameobject.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/model.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/modelchild.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/pointlight.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/spotlight.cpp
	${SRC_GRAPHICS_DIR}/render.cpp
	${SRC_GRAPHICS_DIR}/shader.cpp
	${SRC_GRAPHICS_DIR}/texture.cpp
	${SRC_UTILS_DIR}/fileUtils.cpp
	${SRC_UTILS_DIR}/stringUtils.cpp
)

# Add your external files
list(APPEND SOURCE_FILES
	${EXTERNAL_DIR}/glad/glad.c
	${EXTERNAL_DIR}/imgui/imgui.cpp
	${EXTERNAL_DIR}/imgui/imgui_demo.cpp
	${EXTERNAL_DIR}/imgui/imgui_draw.cpp
	${EXTERNAL_DIR}/imgui/imgui_impl_glfw.cpp
	${EXTERNAL_DIR}/imgui/imgui_impl_opengl3.cpp
	${EXTERNAL_DIR}/imgui/imgui_tables.cpp
	${EXTERNAL_DIR}/imgui/imgui_widgets.cpp
	${EXTERNAL_DIR}/stb_image/stb_image.cpp
)

# Add the executable
add_executable(Game ${SOURCE_FILES} ${RESOURCE_FILE})

# Forces the name to be Game
set_target_properties(Game PROPERTIES OUTPUT_NAME "Game")

target_compile_features(Game PRIVATE cxx_std_20)

# Set the runtime library option
# target_compile_options(Game PRIVATE /MD)

target_include_directories(Game PRIVATE 
	${INCLUDE_DIR}/game
	${INCLUDE_DIR}/game/core
	${INCLUDE_DIR}/game/file
	${INCLUDE_DIR}/game/graphics
	${INCLUDE_DIR}/game/graphics/shapes
	${INCLUDE_DIR}/game/utils
	${INCLUDE_DIR}/external
	${INCLUDE_DIR}/external/assimp
	${INCLUDE_DIR}/external/glad
	${INCLUDE_DIR}/external/glfw
	${INCLUDE_DIR}/external/glm
	${INCLUDE_DIR}/external/glm/detail
	${INCLUDE_DIR}/external/glm/ext
	${INCLUDE_DIR}/external/glm/gtc
	${INCLUDE_DIR}/external/glm/gtx
	${INCLUDE_DIR}/external/glm/simd
	${INCLUDE_DIR}/external/magic_enum
	${INCLUDE_DIR}/external/imgui
	${INCLUDE_DIR}/external/stb_image
)

target_link_libraries(Game PRIVATE external_libs glad ${GLFW_LIBRARY_PATH} ${ASSIMP_LIBRARY_PATH})

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Game PRIVATE GLFW_INCLUDE_NONE)

# Set the subsystem to WINDOWS
#set_target_properties(Game PROPERTIES
#    WIN32_EXECUTABLE TRUE
#)

# Set the entry point (MSVC-specific)
#if (MSVC)
#    set_target_properties(Game PROPERTIES
#       LINK_FLAGS "/ENTRY:mainCRTStartup"
#    )
#endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Game DESTINATION ${CMAKE_INSTALL_BINDIR})

# Create "project" directory in files folder if it doesnt exist
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/files/project)

# Copy "files" directory to the install directory after build
add_custom_command(TARGET Game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:Game>/files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/files 
	$<TARGET_FILE_DIR:Game>/files
)

# Copy assimp dll to the install directory after build
add_custom_command(TARGET Game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Game>/assimp-vc143-mt.dll
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/files/game/external\ dlls/assimp-vc143-mt.dll
    $<TARGET_FILE_DIR:Game>/assimp-vc143-mt.dll
)

# Copy "LICENSE.md" to install directory after build
add_custom_command(TARGET Game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Game>/LICENSE.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/LICENSE.md
    $<TARGET_FILE_DIR:Game>/LICENSE.md
)

# Include CPack to enable packaging
include(CPack)