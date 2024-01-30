cmake_minimum_required (VERSION 3.27)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 10)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso engine" VERSION ${PROJECT_VERSION})

# Reusable message types printed to console
set(eninf "[ENGINE_INFO]")

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")
set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/icon.rc")

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(FILES_DIR "${CMAKE_SOURCE_DIR}/files")

# Manually added source file paths
set(SRC_CORE_DIR "${CMAKE_SOURCE_DIR}/src/engine/core")
set(SRC_FILE_DIR "${CMAKE_SOURCE_DIR}/src/engine/file")
set(SRC_GRAPHICS_DIR "${CMAKE_SOURCE_DIR}/src/engine/graphics")
set(SRC_GRAPHICS_SHAPE_DIR "${CMAKE_SOURCE_DIR}/src/engine/graphics/shapes")
set(SRC_PHYSICS_DIR "${CMAKE_SOURCE_DIR}/src/engine/physics")
set(SRC_UTILS_DIR "${CMAKE_SOURCE_DIR}/src/engine/utils")
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/src/external")

# Add GLAD
add_library(glad STATIC ${SRC_DIR}/external/glad/glad.c)
target_include_directories(glad PUBLIC ${INCLUDE_DIR})

# Add GLFW
set(GLFW_LIBRARY_PATH "${LIB_DIR}/glfw/glfw3.lib")

# Add magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${INCLUDE_DIR}/magic_enum)

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad magic_enum)

# Add your source files
set(SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/engine/main.cpp
	${SRC_CORE_DIR}/console.cpp
	${SRC_CORE_DIR}/core.cpp
	${SRC_CORE_DIR}/errorpopup.cpp
	${SRC_CORE_DIR}/input.cpp
	${SRC_CORE_DIR}/shutdown.cpp
	${SRC_CORE_DIR}/timeManager.cpp
	${SRC_FILE_DIR}/configFile.cpp
	${SRC_GRAPHICS_DIR}/gui/gui.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_console.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_debugmenu.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_inspector.cpp
	${SRC_GRAPHICS_DIR}/gui/gui_projecthierarchy.cpp
	${SRC_GRAPHICS_DIR}/grid.cpp
	${SRC_GRAPHICS_DIR}/render.cpp
	${SRC_GRAPHICS_DIR}/shader.cpp
	${SRC_GRAPHICS_DIR}/texture.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/billboard.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/cube.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/gameobject.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/pointlight.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/selectedobjectborder.cpp
	${SRC_GRAPHICS_SHAPE_DIR}/spotlight.cpp
	${SRC_PHYSICS_DIR}/selectobject.cpp
	${SRC_PHYSICS_DIR}/collision.cpp
	${SRC_UTILS_DIR}/browserUtils.cpp
	${SRC_UTILS_DIR}/fileUtils.cpp
	${SRC_UTILS_DIR}/searchUtils.cpp
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
add_executable(Elypso_engine ${SOURCE_FILES} ${RESOURCE_FILE})

# Forces the name to be Elypso engine
set_target_properties(Elypso_engine PROPERTIES OUTPUT_NAME "Elypso engine")

target_compile_features(Elypso_engine PRIVATE cxx_std_20)

# Set the runtime library option
# target_compile_options(Elypso_engine PRIVATE /MD)

target_include_directories(Elypso_engine PRIVATE 
	${INCLUDE_DIR}/engine
	${INCLUDE_DIR}/engine/core
	${INCLUDE_DIR}/engine/file
	${INCLUDE_DIR}/engine/graphics
	${INCLUDE_DIR}/engine/graphics/gui
	${INCLUDE_DIR}/engine/graphics/shapes
	${INCLUDE_DIR}/engine/physics
	${INCLUDE_DIR}/engine/utils
	${INCLUDE_DIR}/external
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

target_link_libraries(Elypso_engine PRIVATE external_libs glad ${GLFW_LIBRARY_PATH})

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Elypso_engine PRIVATE GLFW_INCLUDE_NONE)

# Set the subsystem to WINDOWS
# set_target_properties(Elypso_engine PROPERTIES
#     WIN32_EXECUTABLE TRUE
# )

# Set the entry point (MSVC-specific)
# if (MSVC)
#     set_target_properties(Elypso_engine PROPERTIES
#        LINK_FLAGS "/ENTRY:mainCRTStartup"
#     )
# endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Elypso_engine DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy "files" directory to the install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:Elypso_engine>/files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/files $<TARGET_FILE_DIR:Elypso_engine>/files
)

# Copy "EULA.md", "LICENSE.md" and "README.md" to install directory after build
add_custom_command(TARGET Elypso_engine POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Elypso_engine>/EULA.md
    COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/EULA.md
    $<TARGET_FILE_DIR:Elypso_engine>/EULA.md
	
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Elypso_engine>/LICENSE.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/LICENSE.md
    $<TARGET_FILE_DIR:Elypso_engine>/LICENSE.md
	
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Elypso_engine>/README.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${PARENT_DIR_PATH}/README.md
    $<TARGET_FILE_DIR:Elypso_engine>/README.md
)

# Include CPack to enable packaging
include(CPack)