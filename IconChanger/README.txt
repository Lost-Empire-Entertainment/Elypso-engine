Disclaimer:

	IconChanger is created and managed by Lost Empire Entertainment. All permissions and restrictions given by the EULA apply to IconChanger.

Description:

	Use IconChanger to change the icon of the engine.
		
	Running IconManager.exe looks for the ico file in your_project_path\Engine\files\icons and attaches it to the executable found in one of three locations:
	
		1) if you compiled the engine from source - your_project_path\Engine\build\Release
		2) if you compiled the engine in Release mode in Visual studio - your_project_path\Engine\out\build\x64-release
		3) if you compiled the engine in Debug mode in Visual studio - your_project_path\Engine\out\build\x64-debug
	
How to build IconChanger from source:

	To compile the IconChanger from source outside of Visual Studio you need to run build.bat with administrative privileges.
	First run ReConfigure CMake to set up CMake to set up installation, then run Build Source Manager to IconChanger.exe.
	You can find IconChanger.exe in one of three locations:
		
		1) if you compiled it from source - your_project_path\IconChanger\build\Release
		2) if you compiled it in Release mode in Visual studio - your_project_path\IconChanger\out\build\x64-release
		3) if you compiled it in Debug mode in Visual studio - your_project_path\IconChanger\out\build\x64-debug
	
Files and folders: 
		
	IconChanger is a standalone exe and does not require any other files and folders to run.
		
	The IconChanger source file is located in the src folder.
		
	There are a few extra other files that are also used by the source manager:
	
		1) build.bat - compiles the IconChanger from source.
		2) CMakeLists.txt - holds all the information CMake needs to compile and pack IconChanger into an exe.
		3) CMakePresets.json - a few presets for the IDE (Visual Studio in our case) to know what build types the IDE can use.
	
Notes:

	your_project_path is the path where you cloned the repository of Elypso engine to.