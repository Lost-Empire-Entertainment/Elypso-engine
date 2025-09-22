@echo off

cd /d "%~dp0"

set "VULKAN_CPP=imgui\backends\imgui_impl_vulkan.cpp"
set "VULKAN_HPP=imgui\backends\imgui_impl_vulkan.h"
set "VULKAN_FOLDER=imgui\backends\vulkan"

if not exist "%VULKAN_CPP%" (
	echo Skipped deleting vulkan cpp because it doesn't exist!
) else (
	del /f /q "%VULKAN_CPP%" >nul
)

if not exist "%VULKAN_HPP%" (
	echo Skipped deleting vulkan hpp because it doesn't exist!
) else (
	del /f /q "%VULKAN_HPP%" >nul
)

if not exist "%VULKAN_FOLDER%" (
	echo Skipped deleting vulkan folder because it doesn't exist!
) else (
	rd /s /q "%VULKAN_FOLDER%" >nul
)

echo Finished cleaning ImGui Vulkan files.