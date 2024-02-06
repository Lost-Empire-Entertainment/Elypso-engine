# Disclaimer

Elypso engine is created and managed by Lost Empire Entertainment. All permissions and restrictions given by the EULA apply to Elypso engine.

# Description

Elypso engine.exe is the executable to launch the engine, build.bat is the batch script used to compile the engine from source - more information can be found about the setup below.

Please be aware that no support has been added yet for Mac or Linux, the engine is only functional on Windows at the current stage and no plans have been made to support the engine on other platforms.
		
Running Elypso engine creates a folder at your_documents_path\Elypso engine with three files:
	
	- config.txt - holds the configuration data of the current settings for Elypso engine.
	- engine_log.txt - writes all in-engine log messages to this external file to help with debugging.
	- imgui.ini - the actual ImGui ini file that holds the data to construct all the ImGui windows of Elypso engine

# Built with

These are the external libraries Elypso engine uses natively, you do not need to download or set them up yourself (except CMake), they come with the engine and are functional out of the box.

* [CMake](https://cmake.org/) - generates build files for the engine
* [GLFW](https://www.glfw.org/) - provides a simple API for creating windows, contexts and surfaces, receiving input and events
* [GLAD](https://glad.dav1d.de/) - simplifies the process of managing OpenGL function pointers
* [GLM](https://github.com/g-truc/glm) - C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specification
* [magic enum](https://github.com/Neargye/magic_enum) - provides static reflection for enums, work with any enum type without any macro or boilerplate code
* [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) - image loading library that is able to load most popular file formats
* [Dear ImGui](https://github.com/ocornut/imgui) - immediate mode GUI library
  
# How to build Elypso engine from source

Before you can compile Elypso engine with the provided build.bat file you should have CMake installed and CMake bin folder added to the environment path.

To compile the Elypso engine from source you need to run build.bat with administrative privileges.
First run Reconfigure CMake to set up CMake, then run Build to build Elypso engine.exe.
You can find Elypso engine.exe in one of three locations:
		
	- if you compiled it from source - your_project_path\Engine\build\Release
	- if you compiled it in Release mode in Visual studio - your_project_path\Engine\out\build\x64-release
	- if you compiled it in Debug mode in Visual studio - your_project_path\Engine\out\build\x64-debug
		
# Files and folders
		
All the compiled versions of Elypso engine come with the cloned files and engine folders.
The origin is found at your_project_path\Engine\files and \engine.
Engine folder has four created folders that are required to go along with all distributed versions of Elypso engine to ensure it runs:
	
	- bat scripts - holds bat scripts that are ran from inside Elypso engine for different interactions.
	- fonts - holds all the actual Elypso engine fonts.
	- icons - holds all the icons the engine uses for its billboards and engine icons
	- shaders - holds all the shaders that Elypso engine and its created games use.

 Files folder has one required folder:

    - textures - holds all the textures that Elypso engine and its created games use.
	
Elypso engine also has three extra files for the engine icons at the root engine folder directory that are required to be added alongside with the required folders:

    - icon.png
    - icon.ico
    - icon.rc
		
The Elypso engine source, header and library files are distributed into three folders:
	
	- src - holds all the Elypso engine and External source scripts.
	- include - holds all the Elypso engine and External header scripts.
	- lib - holds all the External library files.
	
There are a few extra other files that are also used by Elypso engine:
	
	- build.bat - compiles Elypso engine from source.
	- CMakeLists.txt - holds all the information CMake needs to compile and pack Elypso engine into an exe.
	- CMakePresets.json - a few presets for the IDE (Visual Studio in our case) to know what build types the IDE can use.
		
# Notes

	- your_project_path is the path where you cloned the repository of Elypso engine to.
	- your_documents_path is the path to your current user Documents folder.
