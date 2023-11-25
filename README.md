# Elypso engine

Welcome to the main repository of Elypso engine which is an open source C++ OpenGL engine used in Windows. Its main goal will be to make RPG and shooter games and it will be the main game creation engine for my game studio Lost Empire Entertainment once it is production ready.

Join the community server: https://discord.com/invite/3jxDdHJ6SQ

Check out the trello board: https://trello.com/b/hbt6ebCZ/elypso-engine

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

### Installing

```
1) run build.bat to generate the build and install folders with all the important files
2) right click the parent project directory and press open with visual studio to launch the solution
3) build the project from the opened solution file on x64 Release platform

- install folder contains the engine installer which allows creating the exe to run the actual engine itself
- build folder contains the visual studio solution and all the necessary files that allow creating the installer for the project
```

## Built With

* [CMake and cpack](https://cmake.org/) - generates build files for the engine
* [GLFW](https://www.glfw.org/) - provides a simple API for creating windows, contexts and surfaces, receiving input and events
* [GLAD](https://glad.dav1d.de/) - simplifies the process of managing OpenGL function pointers in a cross-platform manner
* [GLM](https://github.com/g-truc/glm) - C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specification
* [Assimp](https://github.com/assimp/assimp) - can import and load over 40 3d file formats into a shared, in-memory format
* [ImGUI](https://github.com/ocornut/imgui) - graphical user interface library for C++
* [NSIS](https://nsis.sourceforge.io/Download) - windows installer generator
* [nlohmann/json](https://github.com/nlohmann/json) - data interchange format that is easy for humans to read and write, and easy for machines to parse and generate

## Authors

* [Greenlaser](https://github.com/greeenlaser) - *Lead developer*

## License

This project is licensed under the GPL-3.0 License - see the [LICENSE.md](LICENSE.md) file for details
