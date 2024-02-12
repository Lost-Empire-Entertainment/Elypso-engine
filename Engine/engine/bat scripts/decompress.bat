@echo off

:: Check if the correct number of arguments is provided
if "%~2"=="" (
	echo Error: Incorrect amount of arguments passed to script! Please include destination path and target path.
	exit /b 1
)

echo Input Path: %~1
echo Destination Path: %~2

:: Decompress the zip package into a regular folder
powershell Expand-Archive -Path "%~1" -DestinationPath "%~2"