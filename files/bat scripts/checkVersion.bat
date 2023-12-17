:: Copyright (C) 2023 Greenlaser
:: This program comes with ABSOLUTELY NO WARRANTY.
:: This is free software, and you are welcome to redistribute it under certain conditions.
:: Read LICENSE.md and EULA.md for more information.

@echo off
setlocal enabledelayedexpansion

set "webPage=https://github.com/Lost-Empire-Entertainment/Elypso-engine/releases"
set "searchString=Elypso engine"
set "outputFile=output.txt"
set "tempFile=temp.txt"

:: Navigate to the directory where your batch script is located
cd /d "%~dp0"

:: Check if the output file exists and delete it
if exist "%outputFile%" (
    del "%outputFile%"
)

:: Search for the specified string in the webpage and output to a temporary file
curl %webPage% | findstr /i /c:"%searchString%" > "%tempFile%"

:: Process the temporary file to extract version numbers between "Elypso engine" and "prototype"
for /f "tokens=* delims=" %%a in ('type "%tempFile%"') do (
    set "line=%%a"
    set "line=!line:*%searchString%=!"
    set "line=!line:prototype=!"

    :: Remove content within <> brackets
    set "line=!line:*<h2>=!"
    for /f "tokens=1,* delims=<>" %%b in ("!line!") do set "line=%%b"

    :: Remove unwanted /li
    set "line=!line:/li=!"

    :: Remove leading and trailing spaces
    for /f "tokens=* delims=" %%c in ("!line!") do set "line=%%c"

    echo !line!
)

:: Delete the temporary file
del "%tempFile%"

endlocal