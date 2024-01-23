Disclaimer:

	Elypso engine is created and managed by Lost Empire Entertainment. All permissions and restrictions given by the EULA apply to Elypso engine.

Description:

	Elypso_engine.exe is the actual engine executable.
	You can find it in one of three locations:
		
		1) if you compiled Elypso engine from source - your_project_path\Engine\build\Release
		2) if you compiled Elypso engine in Release mode in Visual studio - your_project_path\Engine\out\build\x64-release
		3) if you compiled Elypso engine in Debug mode in Visual studio - your_project_path\Engine\out\build\x64-debug
		
	Running Elypso engine creates a folder at your_documents_path\Elypso engine with three files:
	
		1) config.txt - holds the configuration data of the current settings for Elypso engine.
		2) engine_log.txt - writes all in-engine log messages to this external file to help with debugging.
		3) imgui.ini - the actual ImGui ini file that holds the data to construct all the ImGui windows of Elypso engine.
		
Files and folders:
		
	All the compiled versions of Elypso engine come with the cloned files folder.
	The origin is found at your_project_path\Engine\files.
	It has five created folders that are required to go along with all distributed versions of Elypso engine to ensure it runs:
	
		1) bat scripts - holds bat scripts that are ran from inside Elypso engine for different interactions.
		2) fonts - holds all the actual Elypso engine fonts.
		3) icons - holds the png and ico version of the Elypso engine logo.
		4) shaders - holds all the shaders that Elypso engine and its created games use.
		5) textures - holds all the textures that Elypso engine and its created games use.
		
	The Elypso engine source, header and library files are distributed into three folders:
	
		1) src - holds all the Elypso engine and External source scripts.
		2) include - holds all the Elypso engine and External header scripts.
		3) lib - holds all the External library files.
		
	There are a few extra other files that are also used by Elypso engine:
	
		1) build.bat - the "back-end" to SourceManager. Handles all the interactions SourceManager needs to build Elypso engine from source.
		2) CMakeLists.txt - holds all the information CMake needs to compile and pack Elypso engine into an exe.
		3) CMakePresets.json - a few presets for the IDE (Visual Studio in our case) to know what build types the IDE can use.
		
Notes:

	your_project_path is the path where you cloned the repository of Elypso engine to.
	
	your_documents_path is the path to your current user Documents folder.