:: Copyright (C) 2024 Greenlaser
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md and EULA.md for more information.

@echo off
:: Batch script that signs the executables of Elypso engine to ensure they run without triggering false-positive virus errors when running the executables.

:: NOTE: THIS IS CURRENTLY NOT IN USE AND IS ONLY A PLACEHOLDER

set "ssuc=[SIGN_SUCCESS]"
set "serr=[SIGN_EXCEPTION]"

set "CERT_NAME=Lost Empire Entertainment"
set "PROGRAM_NAME=Elypso engine"
set "TIMESTAMP_URL=https://timestamp.digicert.com"

:: The path that is sent to this sign batch file
set "PROGRAM_PATH=%1;%PROGRAM_PATH%"

:: Check if the script is running with administrative privileges
NET SESSION >nul 2>&1
if %errorlevel% neq 0 (
    echo %prexc% This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

if "%1"=="" (
    echo %serr% No path was included with the bat file!
    pause
    exit
)

if exist "%1" (
	:: Check if the path leads to an executable
	if /I "%~x1"==".exe" (
		:: Create a self-signed certificate
		makecert -r -pe -ss PrivateCertStore -n "CN=%CERT_NAME%" %CERT_NAME%.cer

		:: Sign the program
		signtool sign /s PrivateCertStore /n "%CERT_NAME%" /t %TIMESTAMP_URL% /v %PROGRAM_PATH%

		:: Verify the signature
		signtool verify /pa /v %PROGRAM_PATH%
		
		echo %ssuc% Signed %PROGRAM_NAME% successfully!
		pause
		exit
	) else (
		echo %serr% %PATH% is not an executable!
		pause
		exit
	)
) else (
	echo %serr% Path %PATH% is not a valid path!
	pause
	exit
)