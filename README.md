# Elypso engine

Welcome to the main repository of Elypso engine which is an open source C++ OpenGL engine used in Windows. Its main goal will be to make RPG and shooter games and it will be the main game creation engine for my game studio Lost Empire Entertainment once it is production ready.

[Check out the trello board](https://trello.com/b/hbt6ebCZ/elypso-engine)

[Support development on paypal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

```
- Windows 11 (untested on older versions but Windows 10 should work fine too)
- gpu with openGL 4.6 support
- Visual Studio 17 2022 (untested on older VS versions and other IDEs)
- CMake downloaded and set up with CMake/bin folder added to environment path
- NSIS installed with makensis.exe added to environment path
```

### Setup

```
- right-click the source directory folder and click open with visual studio to edit the source code
- run RUN_ME.bat to configure cmake, generate build folder or generate nsis installer
```

### Script layout

```
- src/engine and include/engine contain the actual scripts that were created for the engine
- src/external, include/external and lib are folders containing libraries and scripts that were not created by the developers of this engine

- timestamp.cpp is a small script that just handles adding a timestamp to each console message
- writeConsoleMessage.cpp is a small script that handles how each console message is sent to the console
```

### Logs

```
- cmake configuration log is found at build\logs\cmake_log.txt
- build generation log is found at build\logs\build_log.txt
- cpack log is found at build\logs\cpack_log.txt
- engine log is found at install\engine_log.txt
```

## Built with

* [CMake](https://cmake.org/) - generates build files for the engine
* [GLFW](https://www.glfw.org/) - provides a simple API for creating windows, contexts and surfaces, receiving input and events
* [GLAD](https://glad.dav1d.de/) - simplifies the process of managing OpenGL function pointers in a cross-platform manner
* [GLM](https://github.com/g-truc/glm) - C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specification
* [NSIS](https://nsis.sourceforge.io/Download) - windows installer generator
* [magic enum](https://github.com/Neargye/magic_enum) - provides static reflection for enums, work with any enum type without any macro or boilerplate code
* [ImGUI](https://github.com/ocornut/imgui) - graphical user interface library

## Authors

* [Greenlaser](https://github.com/greeenlaser) - *Lead developer*

## License

This project is licensed under the GPL-3.0 License - see the [license](LICENSE.md) file for details
