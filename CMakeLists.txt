cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Elypso engine")

# Add source to this project's executable.
add_executable (CMakeTarget "src/Main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# CPack configuration
set(CPACK_PACKAGE_NAME "Elypso engine")
set(CPACK_PACKAGE_VERSION "Prototype 1")

# For Windows NSIS installer
set(CPACK_GENERATOR NSIS)  
set(CPACK_NSIS_MODIFY_PATH ON)

# Set the icon for the installer
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/files/icon.ico")

# Specify the start menu shortcut
set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Elypso Engine.lnk' '$INSTDIR\\\\bin\\\\CMakeTarget.exe'")

# Set additional CPack options if needed
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "69")
# set(CPACK_PACKAGE_EXECUTABLES "CMakeTarget" "Elypso Engine")

include(CPack)