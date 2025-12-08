# Prerequisites for building from source

Download Visual Studio 2022 Installer from here:
https://visualstudio.microsoft.com/vs/

once its installed make sure these are enabled when installing Visual Studio 2022:
- desktop development with c++ is enabled
- it has c++ cmake tools enabled
- it has windows 10/11 sdk enabled

And finally unpack prerequisites.7z and then run setup.bat to install cmake and ninja

## How to build from source

The compiled executable and its files will be placed to `/release` and `/debug` in the root folder relative to the build stage. Run `build_windows.bat` to build the game from source.