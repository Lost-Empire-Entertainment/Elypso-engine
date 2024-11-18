cmake_minimum_required (VERSION 3.22)

# Enable Hot Reload for MSVC compilers if supported.
if (WIN32 AND POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Specify the compiler for Linux builds
if (UNIX)
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Elypso hub" VERSION ${PROJECT_VERSION})

# Paths to frequently used directories
get_filename_component(PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY)
set(PARENT_DIR_PATH "${PARENT_DIR}")

if (WIN32)
	set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/icon.rc")
endif()

# Hub files
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# External files
set(EXT_GLAD_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Glad")
set(EXT_GLFW_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/GLFW")
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/GLM")
set(EXT_IMGUI_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/ImGui")
set(EXT_MAGIC_ENUM_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Magic_enum")
set(EXT_STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}/../_external_shared/Stb_image")

# Add GLAD
add_library(glad STATIC ${EXT_GLAD_DIR}/glad.c)
target_include_directories(glad PUBLIC ${EXT_GLAD_DIR}/..)

# Add GLFW
if (WIN32)
set(GLFW_LIBRARY_PATH "${EXT_GLFW_DIR}/glfw3.lib")
else()
find_package(glfw3 REQUIRED)
endif()

# Add x11 (linux only)
if (UNIX)
find_package(X11 REQUIRED)
endif()

# Add magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${EXT_MAGIC_ENUM_DIR})

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad magic_enum)

# Add your source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
	${CMAKE_SOURCE_DIR}/src/*.cpp
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

# Build as hub executable
if (WIN32)
	add_executable(Elypso_hub ${SOURCE_FILES} ${RESOURCE_FILE})
else()
	add_executable(Elypso_hub ${SOURCE_FILES})
endif()

# Forces the name to be Elypso hub
set_target_properties(Elypso_hub PROPERTIES OUTPUT_NAME "Elypso hub")

target_compile_features(Elypso_hub PRIVATE cxx_std_20)

target_include_directories(Elypso_hub PRIVATE 
	${INCLUDE_DIR}
	${EXT_GLAD_DIR}
	${EXT_GLFW_DIR}
	${EXT_GLM_DIR}
	${EXT_MAGIC_ENUM_DIR}
	${EXT_IMGUI_DIR}
	${EXT_STB_IMAGE_DIR}
)

if (WIN32)
target_link_libraries(Elypso_hub PRIVATE external_libs ${GLFW_LIBRARY_PATH})
else()
target_link_libraries(Elypso_hub PRIVATE external_libs glfw ${X11_LIBRARIES})
endif()

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Elypso_hub PRIVATE GLFW_INCLUDE_NONE)

# Set the subsystem to WINDOWS
#set_target_properties(Elypso_hub PROPERTIES
#    WIN32_EXECUTABLE TRUE
#)

# Set the entry point (MSVC-specific)
#if (MSVC)
#	set_target_properties(Elypso_hub PROPERTIES
#    LINK_FLAGS "/ENTRY:mainCRTStartup"
#)
#endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Elypso_hub DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy "files" directory to the install directory after build
add_custom_command(TARGET Elypso_hub POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:Elypso_hub>/files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/files $<TARGET_FILE_DIR:Elypso_hub>/files
)

set(COPY_TARGETS LICENSE.md README.md SECURITY.md LIBRARIES.md)
foreach(FILE ${COPY_TARGETS})
  add_custom_command(TARGET Elypso_hub POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
	$<TARGET_FILE_DIR:Elypso_hub>/${FILE}
	COMMAND ${CMAKE_COMMAND} -E copy
	${PARENT_DIR_PATH}/${FILE}
	$<TARGET_FILE_DIR:Elypso_hub>/${FILE}
  )
endforeach()

# Include CPack to enable packaging
include(CPack)