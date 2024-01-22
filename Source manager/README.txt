Disclaimer:

	This software is created and managed by Lost Empire Entertainment. All permissions and restrictions given by the EULA apply to this software.

Description:

	Use this software to install the engine from source, clean the engine files if you no longer wish to use the engine and clean the visual studio files if you need to recreate the project in visual studio.
		
	Running the source manager exe creates an imgui.ini file at the same path where the exe is at. It holds the data to construct all the ImGui windows of the source manager.
	
How to use:

	Reconfigure CMake - Sets up engine installer, also required to run when source or header files are added or removed.
	
	Install engine - Intalls the engine to your_project_path\Engine\build\Release. All files and folders included in Release folder are required to be included alongside with the executable when distributing the engine.
	
	Clean engine - finds and deletes the engine documents folder at your_documents_path\Elypso engine.
	
	Clean Visual Studio - finds and deletes the visual studio folder at your_project_path\Engine\out.
	
How to build source manager exe from source:

	To compile the Source manager from source you need to run build.bat with administrative privileges.
	First run ReConfigure CMake to set up CMake to know what needs to be installed, then run Build Source Manager to build the actual Source Manager executable.
	You can find it in one of three locations:
		
		1) if you compiled it from source - your_project_path\Source manager\build\Release
		2) if you compiled it in Release mode in Visual studio - your_project_path\Source manager\out\build\x64-release
		3) if you compiled it in Debug mode in Visual studio - your_project_path\Source manager\out\build\x64-debug
	
Files and folders: 
		
	All the compiled versions of the source manager come with the cloned files folder.
	The origin is found at your_project_path\Source manager\files.
	It has one created folder that is required to go along with all distributed versions of the source manager to ensure it runs:
	
		1) fonts - holds all the actual source manager fonts.
		
	The source manager source, header and library files are distributed into three folders:
	
		1) src - holds all the Source manager and External source scripts.
		2) include - holds all the Source manager and External header scripts.
		3) lib - holds all the External library files.
		
	There are a few extra other files that are also used by the source manager:
	
		1) build.bat - compiles the Source manager exe from source.
		2) CMakeLists.txt - holds all the information CMake needs to compile and pack the source manager into an exe.
		3) CMakePresets.json - a few presets for the IDE (Visual Studio in our case) to know what build types the IDE can use.
	
Notes:

	your_project_path is the path where you cloned the repository of the entire engine to.
	
	your_documents_path is the path to your current user Documents folder.