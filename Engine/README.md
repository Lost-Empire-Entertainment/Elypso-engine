# Disclaimer

	Elypso engine is created and managed by Lost Empire Entertainment. All permissions and restrictions given by the EULA apply to Elypso engine.

# Description

	Elypso_engine.exe is the actual engine executable.
		
	Running Elypso engine creates a folder at your_documents_path\Elypso engine with three files:
	
		```
		- config.txt - holds the configuration data of the current settings for Elypso engine.
		- engine_log.txt - writes all in-engine log messages to this external file to help with debugging.
		- imgui.ini - the actual ImGui ini file that holds the data to construct all the ImGui windows of Elypso engine.
		```
		
# How to build Elypso engine from source

	To compile the Elypso engine from source outside of Visual Studio you need to run build.bat with administrative privileges.
	First run ReConfigure CMake to set up CMake, then run Build to build Elypso engine.exe.
	You can find Elypso engine.exe in one of three locations:
		
		```
		- if you compiled it from source - your_project_path\Elypso_engine\build\Release
		- if you compiled it in Release mode in Visual studio - your_project_path\Elypso_engine\out\build\x64-release
		- if you compiled it in Debug mode in Visual studio - your_project_path\Elypso_engine\out\build\x64-debug
		```
		
# Files and folders
		
	All the compiled versions of Elypso engine come with the cloned files folder.
	The origin is found at your_project_path\Engine\files.
	It has five created folders that are required to go along with all distributed versions of Elypso engine to ensure it runs:
	
		```
		- bat scripts - holds bat scripts that are ran from inside Elypso engine for different interactions.
		- fonts - holds all the actual Elypso engine fonts.
		- icons - holds the png and ico version of the Elypso engine logo.
		- shaders - holds all the shaders that Elypso engine and its created games use.
		- textures - holds all the textures that Elypso engine and its created games use.
		```
		
	The Elypso engine source, header and library files are distributed into three folders:
	
		```
		- src - holds all the Elypso engine and External source scripts.
		- include - holds all the Elypso engine and External header scripts.
		- lib - holds all the External library files.
		```
	
	There are a few extra other files that are also used by Elypso engine:
	
		```
		- build.bat - compiles Elypso engine from source.
		- CMakeLists.txt - holds all the information CMake needs to compile and pack Elypso engine into an exe.
		- CMakePresets.json - a few presets for the IDE (Visual Studio in our case) to know what build types the IDE can use.
		```
		
# Notes

	your_project_path is the path where you cloned the repository of Elypso engine to.
	
	your_documents_path is the path to your current user Documents folder.