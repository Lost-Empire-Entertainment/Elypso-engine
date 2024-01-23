Disclaimer:

	SourceManager is created and managed by Lost Empire Entertainment. All permissions and restrictions given by the EULA apply to SourceManager.

Description:

	Use SourceManager to install Elypso engine from source, clean Elypso engine files if you no longer wish to use Elypso engine and clean the Visual Studio files if you need to recreate the project in Visual Studio.
		
	Running SourceManager creates an imgui.ini file at the same path where SourceManager.exe is at. It holds the data to construct all the ImGui windows of SourceManager.
	
How to use:

	Reconfigure CMake - Sets up Elypso engine installer, also required to run when source or header files are added or removed.
	
	Install engine - Intalls Elypso engine to your_project_path\Engine\build\Release. All files and folders included in Release folder are required to be included alongside with SourceManager.exe when distributing SourceManager.
	
	Clean engine - finds and deletes Elypso engine folder at your_documents_path.
	
	Clean Visual Studio - finds and deletes .vs and out folders your_project_path\Engine.
	
How to build source manager exe from source:

	To compile SourceManager from source outside of Visual Studio you need to run build.bat with administrative privileges.
	First run ReConfigure CMake to set up CMake to set up installation, then run Build Source Manager to build SourceManager.exe.
	You can find SourceManager.exe in one of three locations:
		
		1) if you compiled it from source - your_project_path\Source manager\build\Release
		2) if you compiled it in Release mode in Visual studio - your_project_path\Source manager\out\build\x64-release
		3) if you compiled it in Debug mode in Visual studio - your_project_path\Source manager\out\build\x64-debug
	
Files and folders: 
		
	All the compiled versions of SourceManager.exe come with the cloned files folder.
	The origin is found at your_project_path\SourceManager\files.
	It has one created folder that is required to go along with all distributed versions of SourceManager to ensure it runs:
	
		1) fonts - holds all the actual SourceManager fonts.
		
	The SourceManager source, header and library files are distributed into three folders:
	
		1) src - holds all the SourceManager and External source scripts.
		2) include - holds all the SourceManager and External header scripts.
		3) lib - holds all the External library files.
		
	There are a few extra other files that are also used by SourceManager:
	
		1) build.bat - compiles SourceManager exe from source.
		2) CMakeLists.txt - holds all the information CMake needs to compile and pack SourceManager into an exe.
		3) CMakePresets.json - a few presets for the IDE (Visual Studio in our case) to know what build types the IDE can use.
	
Notes:

	your_project_path is the path where you cloned the repository of Elypso engine to.
	
	your_documents_path is the path to your current user Documents folder.