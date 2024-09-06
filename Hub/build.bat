::Copyright (C) 2024 Lost Empire Entertainment
::This program comes with ABSOLUTELY NO WARRANTY.
::This is free software, and you are welcome
::to redistribute it under certain conditions.
::Read LICENSE.md for more information.

::==================================================
::DESCRIPTION:
::
::This file is responsible for managing
::the configuration, build and cleanup
::of Elypso hub, Elypso engine or
::the Game template for Elypso engine,
::depending on the program folder this was placed into.
::
::Warning: Modifying the file structure of any of the
::files and folders for this program folder
::or modifying this compiler file can break things
::if modified incorrectly! Edit at your own risk. 
::
::==================================================

@echo off

title Compiler

::==================================================
::reusable message types printed to console
::==================================================
set "prexc=[PROGRAM_EXCEPTION]"
set "prinf=[PROGRAM_INFO]"
set "prcln=[PROGRAM_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmexc=[CMAKE_EXCEPTION]"
set "cmsuc=[CMAKE_SUCCESS]"
set "cpinf=[CPACK_INFO]"
set "cpexc=[CPACK_EXCEPTION]"
set "cpsuc=[CPACK_SUCCESS]"

::build folder is assigned inside the folder
::relative to where this file was ran at
set "buildPath=%~dp0build"

::==================================================
::first parameter starts a function
::==================================================
set "p1=%1"

::runs help function
if "%p1%"=="help" (
	goto help
)

::runs cmake configuration (rebuilds the program from scratch)
if "%p1%"=="reset" (
	echo ---- %prinf% Chose 'reset' as first parameter. Compiler will rebuild program from scratch. CMake configure mode has been set to 'Release'.
	echo.
	goto cmake
)

::runs clean hub function
if "%p1%"=="clhub" (
	echo ---- %prinf% Chose 'clgame' as first parameter. Compiler will clean Elypso hub files.
	echo.
	goto clhub
)
::runs clean engine function
if "%p1%"=="cleng" (
	echo ---- %prinf% Chose 'clgame' as first parameter. Compiler will clean Elypso engine files.
	echo.
	goto cleng
)
::runs clean game template function
if "%p1%"=="clgame" (
	echo ---- %prinf% Chose 'clgame' as first parameter. Compiler will clean game template files.
	echo.
	goto clgame
)

::runs compile function if first parameter is either 'y' or 'n'
if "%p1%"=="y" (
	echo ---- %prinf% Chose 'y' as first parameter. Compiler will wait before closing.
	echo.
	goto comp
)
if "%p1%"=="n" (
	echo ---- %prinf% Chose 'n' as first parameter. Compiler will not wait before closing.
	echo.
	goto comp
)

::==================================================
::if first parameter was empty or invalid
::==================================================
echo ---- %prexc% First parameter '%p1%' is empty or invalid. Please run 'help' to find what parameters are available for this file.
pause
exit /b 1

::==================================================
::lists all parameters and how to use them
::==================================================
:help
echo.
echo ==================================================
echo.

echo Full list of parameters

echo.

echo help - lists all parameters

echo ----------

echo reset - fully rebuilds the program from scratch by running cmake configuration first

echo ----------

echo clhub - cleans all hub files if they exist
echo cleng - cleans all engine files if they exist
echo clgame - cleans all game template files if they exist

echo.

echo Warning: All three cl commands clean the build, out (for Visual Studio) and document folders relative to the chosen parameter if they exist. This cannot be undone!

echo ----------

echo If you are compiling the program then you must follow the setup below, both first and second parameter need to be filled for this program to work correctly.

echo.

echo Parameter 1 must either be 'y' or 'n' (yes or no) - this keeps the compiler window open after compilation is done, useful if compiling directly from this file.

echo.

echo Parameter 2 must either be 'd' or 'r' (Debug or Release) - this sets the configuration that cmake will use for this program as Debug or Release

echo.
echo ==================================================
echo.

pause
exit

::==================================================
::cleans hub build and documents folders
::==================================================
:clhub
set "documentsPath=%USERPROFILE%\Documents\Elypso hub"

echo ---- %prinf% Attempting to delete build folder at '%buildPath%'
echo.
if exist "%buildPath%" (
	echo %prcln% Deleted folder: build
	rd /s /q "%buildPath%"
)

echo ---- %prinf% Attempting to delete documents folder at '%documentsPath%'
echo.
if exist "%documentsPath%" (
	echo %prcln% Deleted folder: Documents/My Games/Elypso hub
	rd /s /q "%documentsPath%"
)

pause
exit

::==================================================
::cleans engine build and documents folders
::==================================================
:cleng
set "documentsPath=%USERPROFILE%\Documents\Elypso engine"

echo ---- %prinf% Attempting to delete build folder at '%buildPath%'
echo.
if exist "%buildPath%" (
	echo %prcln% Deleted folder: build
	rd /s /q "%buildPath%"
)

echo ---- %prinf% Attempting to delete documents folder at '%documentsPath%'
echo.
if exist "%documentsPath%" (
	echo %prcln% Deleted folder: Documents/Elypso engine
	rd /s /q "%documentsPath%"
)

pause
exit

::==================================================
::cleans game template build and documents folders
::==================================================
:clgame
::optional third parameter for game name
set "p3=%~3"
set "gameName=123123 123"
if not "%p3%"=="" (
    echo ---- %prinf% Set optional game name to '%gameName%'.
    set "gameName=%p3%"
)

set "documentsPath=%USERPROFILE%\Documents\My Games\"%gameName%""
	
echo ---- %prinf% Attempting to delete build folder at '%buildPath%'
echo.
if exist "%buildPath%" (
	echo %prcln% Deleted folder: build
	rd /s /q "%buildPath%"
)

echo ---- %prinf% Attempting to delete documents folder at '%documentsPath%'
echo.
if exist "%documentsPath%" (
	echo %prcln% Deleted folder: Documents/My Games/%gameName%
	rd /s /q "%documentsPath%"
)

pause
exit

::==================================================
::checks if user chose to wait at end before closing
::==================================================
:comp
::checks if user chose to wait before end or not
if not "%p1%"=="y" (
	if not "%p1%"=="n" (
		echo ---- %prexc% Parameter 1 '%p1%' is invalid! Please run 'help' to find what parameters are available for this file.
		pause
		exit /b 1
	)
)

::checks what build type user chose
set "p2=%2"
if "%p2%"=="" (
	echo ---- %prexc% Parameter 2 is empty! Please run 'help' to find what parameters are available for this file.
	pause
	exit /b 1
)
if not "%p2%"=="r" (
	if not "%p2%"=="d" (
		echo ---- %prexc% Parameter 2 '%p2%' is invalid! Please run 'help' to find what parameters are available for this file.
		pause
		exit /b 1
	)
)

if "%p2%"=="r" (
	set "type=Release"
)
if "%p2%"=="d" (
	set "type=Debug"
)
echo ---- %prinf% Chose '%p2%' as second parameter. CMake will now configure the program to be built in %type% mode.
echo.

::==================================================
::starts building the program. this is ran first 
::in case the program doesnt need to be configured
::because it already has been configured beforehand
::==================================================
:build

::changes to the script directory
cd /d "%~dp0"
	
if not exist "%buildPath%" (
	echo ---- %prinf% Did not find build folder - CMake will be reconfigured.
	echo.
	goto cmake
) else (
	cd "%buildPath%"
		
	::builds the project
	echo ---- %cminf% Started building program
	echo.
	cmake --build . --config Release -- /m
	
	if %errorlevel% neq 0  (
		echo ---- %cmexc% Build failed because executable did not get generated properly.
		if "%p1%"=="y" (
			pause
		)
		exit
	) else (
		echo ---- %cmsuc% Build succeeded!
		if "%p1%"=="y" (
			pause
		)
		if "%p1%"=="reset" (
			pause
		)
		exit
	)
)

::==================================================
::starts configuring the program
::==================================================
:cmake
::changes to the script directory
cd /d "%~dp0"
	
::cleans and remakes the build directory before configuration
::this is essential to ensure the program has a clean slate
::and will not conflict with any existing files or folders
if exist "%buildPath%" (
	echo ---- %prcln% Deleted folder: build
	rd /s /q "%buildPath%"
)
mkdir "%buildPath%"
cd "%buildPath%"

::configures the program and sets build type
echo ---- %cminf% Started configuring program
echo.
if "%type%"=="" (
	set "type=Release"
)
cmake -A x64 -DCMAKE_BUILD_TYPE="%type%" ..

if %errorlevel% neq 0 (
	echo ---- %cmexc% CMake configuration failed.
	echo.
	if "%p1%"=="y" (
		pause
	)
	exit
) else (
	echo ---- %cmsuc% Cmake configuration succeeded!
	echo.
	goto build
)