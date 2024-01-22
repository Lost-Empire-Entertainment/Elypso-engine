Disclaimer:

	This software is created and managed by Lost Empire Entertainment. All permissions and restrictions given by the EULA apply to this software.

Description:

	Elypso_engine.exe is the actual engine executable.
	You can find it in one of three locations:
		
		1) if you compiled the engine from source - your_project_path\Engine\build\Release
		2) if you compiled the engine in Release mode in Visual studio - your_project_path\Engine\out\build\x64-release
		3) if you compiled the engine in Debug mode in Visual studio - your_project_path\Engine\out\build\x64-debug
		
	Running the engine exe creates a folder at your_documents_path\Elypso engine.
	This is where three files are stored to:
	
		1) config.txt - holds the configuration data of the current settings for the engine.
		2) engine_log.txt - writes all in-engine log messages to this external file to help with debugging.
		3) imgui.ini - the actual ImGui ini file that holds the data to construct all the ImGui windows of the engine.
		
Files and folders:
		
	All the compiled versions of the engine come with the cloned files folder.
	The origin is found at your_project_path\Engine\files.
	It has five created folders that are required to go along with all distributed versions of the engine to ensure it runs:
	
		1) bat scripts - holds bat scripts that are ran from inside the engine for different interactions.
		2) fonts - holds all the actual engine fonts.
		3) icons - holds the png and ico version of the engine logo.
		4) shaders - holds all the shaders that the engine and created games use.
		5) textures - holds all the textures that the engine and created games use.
		
	The engine source, header and library files are distributed into three folders:
	
		1) src - holds all the Engine and External source scripts.
		2) include - holds all the Engine and External header scripts.
		3) lib - holds all the External library files.
		
	There are a few extra other files that are also used by the engine:
	
		1) build.bat - the "back-end" to Source manager. Handles all the interactions the Source manager needs.
		2) CMakeLists.txt - holds all the information CMake needs to compile and pack the engine into an exe.
		3) CMakePresets.json - a few presets for the IDE (Visual Studio in our case) to know what build types the IDE can use.
		
Notes:

	your_project_path is the path where you cloned the repository of the entire engine to.
	
	your_documents_path is the path to your current user Documents folder.