@echo off
:: Batch script to build the executable and create the installer for the engine

:: Reusable message types printed to console
set "enerr=[ENGINE_ERROR]"
set "encln=[ENGINE_CLEANUP]"
set "cminf=[CMAKE_INFO]"
set "cmerr=[CMAKE_ERROR]"
set "cmsuc=[CMAKE_SUCCESS]"
set "cpinf=[CPACK_INFO]"
set "cperr=[CPACK_ERROR]"
set "cpsuc=[CPACK_SUCCESS]"
set "inst=Elypso engine installer.exe"
set "ifol=Elypso engine installer"
set "exe=Elypso_engine.exe"

:: Can not run build.bat if no command was inserted
if "%1%" == "" (
	echo %enerr% Please run RUN_ME.bat to choose what actions to do with this project.
	pause
	exit
)

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %enerr% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

::
:: START CMAKE CONFIGURATION WITH "build.bat cmake_config" COMMAND
::

if "%1" == "cmake_config" (
	:: Change to the script directory
	cd /d "%~dp0"

	:: Clean the build directory before configuration
	if exist "build" (
		echo %encln% Deleted folder: build
		rd /s /q build
	)
	mkdir build
	cd build

	echo %cminf% Started CMake configuration.

	:: Configure the project (Release build) and generate NSIS installer
	cmake -DCMAKE_BUILD_TYPE=Release -DCPACK_GENERATOR=NSIS ..

	if %errorlevel% neq 0 (
		echo %cmerr% CMake configuration failed.
	) else (
		echo %cmsuc% Cmake configuration succeeded!
	)
)

::
:: START BUILD WITH "build.bat build" COMMAND
::

if "%1" == "build" (
	:: Change to the script directory
	cd /d "%~dp0"
	
	:: Build folder must exist to run build generation
	if not exist build (
		echo %enerr% Did not find build folder. Please run cmake configuration from RUN_ME.bat.
		pause
		exit
	) else (
		cd build

		:: Build the project
		echo %cminf% Started build generation.
		mkdir logs
		cmake --build . --config Release > logs\build_log.txt 2>&1

		if %errorlevel% neq 0 (
			echo %cmerr% Build failed because %exe% did not get generated properly. Check build/logs/build_log.txt for more details.
		) else (
			if exist "%~dp0\build\Release\%exe%" (
				echo %cmsuc% Build succeeded!
			) else (
				echo %cmerr% Build failed because %exe% did not get generated properly. Check build/logs/build_log.txt for more details.
			)
		)
	)
)

::
:: START INSTALL WITH "build.bat install" COMMAND
::

if "%1" == "install" (
	:: Change to the script directory
	cd /d "%~dp0"
	
	:: Build folder must exist to run install generation
	if not exist build (
		echo %eninf% Did not find build folder. Please run cmake configuration and build from RUN_ME.bat.
		pause
		exit
	) else (
		:: If install folder exists then delete it to always ensure a fresh installer is created
		if exist install (
			rd /s /q install
			echo %encln% Deleted folder: install
		)

		mkdir install

		cd build

		echo %cpinf% Started CPack configuration.

		:: Package the project using CPack with the custom configuration file
		cpack -C Release > "logs\cpack_log.txt" 2>&1

		if %errorlevel% neq 0 (
			echo %cperr% CPack packaging failed because %inst% did not get generated properly. Check build/logs/build_log.txt and build/logs/cpack_log.txt for more details.
		) else (
			if exist "%~dp0\install\_CPack_Packages\win64\NSIS\%inst%" (
				echo %cpsuc% CPack packaging succeeded!
			
				:: Move installed exe to install folder and delete cpack packages folder
				cd ../install
				move "%~dp0\install\_CPack_Packages\win64\NSIS\%ifol%\bin\%exe%" "%~dp0\install"
				echo %encln% Moved file: %exe% to install
				rd /s /q "_CPack_Packages"
				echo %encln% Deleted folder: install/_CPack_Packages
			) else (
				echo %cperr% CPack packaging failed because %inst% did not get generated properly. Check build/logs/build_log.txt and build/logs/cpack_log.txt for more details.
			)
		)
	)
)

pause
exit