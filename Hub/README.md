# Disclaimer

Elypso hub is created and managed by Lost Empire Entertainment. All permissions and restrictions given by the license apply to Elypso hub.

# Description

Elypso hub.exe is the executable to launch the hub, build.bat is the batch script used to compile the hub from source - more information can be found about the setup below.

Please be aware that no support has been added yet for Mac or Linux, the hub is only functional on Windows at the current stage and no plans have been made to support the hub on other platforms.
		
Running Elypso hub creates a folder at your_documents_path\Elypso hub with one file:
	
	- imgui.ini - the actual ImGui ini file that holds the data to construct all the ImGui windows of Elypso hub

# Built with

These are the external libraries Elypso hub uses natively, you do not need to download or set them up yourself (except CMake), they come with Elypso hub and are functional out of the box.

* [CMake](https://cmake.org/) - generates build files for the engine
* [GLFW](https://www.glfw.org/) - provides a simple API for creating windows, contexts and surfaces, receiving input and events
* [GLAD](https://glad.dav1d.de/) - simplifies the process of managing OpenGL function pointers
* [GLM](https://github.com/g-truc/glm) - C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specification
* [magic enum](https://github.com/Neargye/magic_enum) - provides static reflection for enums, work with any enum type without any macro or boilerplate code
* [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) - image loading library that is able to load most popular file formats
* [Dear ImGui](https://github.com/ocornut/imgui) - immediate mode GUI library
  
# How to build Elypso hub from source

Before you can compile Elypso hub with the provided build.bat file you should have CMake installed and CMake bin folder added to the environment path.

To compile the Elypso hub from source you need to run build.bat with administrative privileges.
First run Reconfigure CMake to set up CMake, then run Build to build Elypso hub.exe.
You can find Elypso hub.exe in one of three locations:
		
	- if you compiled it from source - your_project_path\Hub\build\Release
	- if you compiled it in Release mode in Visual studio - your_project_path\Hub\out\build\x64-release
	- if you compiled it in Debug mode in Visual studio - your_project_path\Hub\out\build\x64-debug
		
# Files and folders
		
All the compiled versions of Elypso hub come with the cloned files folder.
The origin is found at your_project_path\Hub\files.
It has one created folder that is required to go along with all distributed versions of Elypso hub to ensure it runs:
	
	- fonts - holds all the actual Elypso hub fonts.

Elypso hub also has three extra files for the engine icons at the root files folder directory that are required to be added alongside with the files folder:

    - icon.png
    - icon.ico
    - icon.rc
		
The Elypso hub source, header and library files are distributed into three folders:
	
	- src - holds all the Elypso hub and External source scripts.
	- include - holds all the Elypso hub and External header scripts.
	- lib - holds all the External library files.
	
There are a few extra other files that are also used by Elypso hub:
	
	- build.bat - compiles Elypso hub from source.
	- CMakeLists.txt - holds all the information CMake needs to compile and pack Elypso hub into an exe.
	- CMakePresets.json - a few presets for the IDE (Visual Studio in our case) to know what build types the IDE can use.
		
# Notes

	- your_project_path is the path where you cloned the repository of Elypso engine to.
	- your_documents_path is the path to your current user Documents folder.
