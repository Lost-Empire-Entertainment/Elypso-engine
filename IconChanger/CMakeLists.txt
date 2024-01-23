# Copyright (C) 2024 Greenlaser
# This program comes with ABSOLUTELY NO WARRANTY.
# This is free software, and you are welcome to redistribute it under certain conditions.
# Read LICENSE.md and EULA.md for more information.

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

project ("IconChanger" VERSION ${PROJECT_VERSION})

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# Add your source files
set(SOURCE_FILES
	${SRC_DIR}/main.cpp
)

# Add the executable
add_executable (IconChanger ${SOURCE_FILES})

# Set the subsystem to WINDOWS
# set_target_properties(IconChanger PROPERTIES
#     WIN32_EXECUTABLE TRUE
# )

# Set the entry point (MSVC-specific)
# if (MSVC)
#     set_target_properties(IconChanger PROPERTIES
#        LINK_FLAGS "/ENTRY:mainCRTStartup"
#     )
# endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS IconChanger DESTINATION ${CMAKE_INSTALL_BINDIR})

# Include CPack to enable packaging
include(CPack)