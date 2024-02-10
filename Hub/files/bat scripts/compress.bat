@echo off

:: Check if the correct number of arguments is provided
if "%~2"=="" (
	echo Error: Incorrect amount of arguments passed to script! Please include destination path and target path.
	exit /b 1
)

:: Compress the folder into a zip package
powershell Compress-Archive -Path "%~1" -DestinationPath "%~2"