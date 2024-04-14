# Disclaimer

The game template is created by ELypso engine but is free to use in any way described in license.md.

# Description

Game.exe is the executable to launch the engine, build.bat is the batch script used to compile the game from source, this is also done in the engine so you dont have to compile the game the same way the engine is compiled - more information can be found about the setup below.

Please be aware that no support has been added yet for Mac or Linux, the game template is only functional on Windows at the current stage and no plans have been made to support the game template on other platforms.
		
Running the game creates a folder at your_documents_path\Game with two files:
	
	- config.txt - holds the configuration data of the current settings for the game.
	- game_log.txt - writes all in-game log messages to this external file to help with debugging.

# Built with

These are the external libraries the game template uses natively, you do not need to download or set them up yourself (except CMake), they come with the game and are functional out of the box.

* [CMake](https://cmake.org/) - generates build files for the game
* [GLFW](https://www.glfw.org/) - provides a simple API for creating windows, contexts and surfaces, receiving input and events
* [GLAD](https://glad.dav1d.de/) - simplifies the process of managing OpenGL function pointers
* [GLM](https://github.com/g-truc/glm) - C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specification
* [magic enum](https://github.com/Neargye/magic_enum) - provides static reflection for enums, work with any enum type without any macro or boilerplate code
* [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) - image loading library that is able to load most popular file formats
* [Dear ImGui](https://github.com/ocornut/imgui) - immediate mode GUI library
* [Assimp](https://github.com/assimp/assimp) - loads 40+ 3D file formats into one unified and clean data structure
  
# How to build the game from source

Before you can compile the game with the provided build.bat file you should have CMake installed and CMake bin folder added to the environment path.

To compile the game from source you need to run build.bat with administrative privileges.
First run Reconfigure CMake to set up CMake, then run Build to build Game.exe.
You can find Game.exe in one of three locations:
		
	- if you compiled it from source - your_project_path\Game\build\Release
	- if you compiled it in Release mode in Visual studio - your_project_path\Game\out\build\x64-release
	- if you compiled it in Debug mode in Visual studio - your_project_path\Game\out\build\x64-debug
		
# Files and folders
		
All the compiled versions of the game come with the cloned files folder.
The origin is found at your_project_path\Game\files.
Inside it are two folders

	- game - holds the files the engine needs to function normally
	- project - holds the files the currently opened project uses, remains empty if no project is open
	
The game folder has several files and folders that are required to go along with all distributed versions of the game:
	
	- bat scripts - holds bat scripts that are ran from inside the game for different interactions
	- external dlls - dlls that are necessary to run the exe
	- fonts - holds all the actual game fonts
	- icons - holds all the icons the game uses for its billboards and game icons
	- models - holds all template models the game uses
	- shaders - holds all the shaders that the game uses
	- textures - holds all the textures that the game uses
	- icon.ico - exe icon file
	- icon.png - exe icon file
	- icon.rc - data for attaching icon.ico to exe
	
The project folder has a folder for each scene and a project.txt file which tells the game which project and scene to load when opening the game. The last opened scene is the one that is always opened every time the project is opened again.
	
The game also has three extra files for the game icons at the root files folder directory that are required to be added alongside with the required folders.
		
The game source, header and library files are distributed into three folders:
	
	- src - holds all the game and External source scripts.
	- include - holds all the game and External header scripts.
	- lib - holds all the External library files.
	
There are a few extra other files that are also used by the game:
	
	- build.bat - compiles the game from source.
	- CMakeLists.txt - holds all the information CMake needs to compile and pack the game into an exe.
	- CMakePresets.json - a few presets for the IDE (Visual Studio in our case) to know what build types the IDE can use.
		
# Notes

	- your_project_path is the path where you cloned the repository of the game to.
	- your_documents_path is the path to your current user Documents folder.
