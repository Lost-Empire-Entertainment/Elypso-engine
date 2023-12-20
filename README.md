# Elypso engine

Elypso engine is an open source C++ OpenGL engine used in Windows. The engine is entirely "codeless" where the end user does not need to write a single line of code to make games with the engine. Everything is handled by the engine itself and the user only needs to know how to use the available packages. Packages are parts of the engine the end user can add and remove to change parts of the engine. Packages are completely optional and none of them need to be activated to run the engine itself but adding new packages or removing existing ones will modify what can be used to make games with in the engine.

[Join the community server](https://discord.gg/wPYCSSenX2)

[Check out the trello board](https://trello.com/b/hbt6ebCZ/elypso-engine)

[Support development on paypal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

### Prerequisites

```
- Windows 11 (untested on older versions but Windows 10 should work fine too)
- gpu with openGL 4.6 support
- Visual Studio 17 2022 (untested on older VS versions and other IDEs)
- CMake downloaded and set up with CMake/bin folder added to environment path
```

### Setup

```
- the recommended path to download the repository to is C:\Documents\Cpp projects\Elypso engine, you can name Cpp projects to whatever you like. Other paths are untested and might not work.

- source code can be edited by right-clicking the Elypso engine folder and opening with visual Studio

how to use RUN_ME.bat:
- reconfigure cmake (always run this when first opening the visual studio project or if you add or remove source or header files)
- install (builds the executable and copies all necessary files over to program files)
- clean visual studio (deletes .vs and out folders)
- clean engine (deletes build, program files engine folder and documents engine folder)

### Logs and configuration files

```
- cmake configuration log is found at build\logs\cmake_log.txt
- build generation log is found at build\logs\build_log.txt
- cpack log is found at build\logs\cpack_log.txt
- engine log is found at Documents\Elypso engine\engine_log.txt
- config file is found at Documents\Elypso engine\config.txt
- imgui ini file is found at Documents\Elypso engine\imgui.imi
```

## Built with

* [CMake](https://cmake.org/) - generates build files for the engine
* [GLFW](https://www.glfw.org/) - provides a simple API for creating windows, contexts and surfaces, receiving input and events
* [GLAD](https://glad.dav1d.de/) - simplifies the process of managing OpenGL function pointers in a cross-platform manner
* [GLM](https://github.com/g-truc/glm) - C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specification
* [magic enum](https://github.com/Neargye/magic_enum) - provides static reflection for enums, work with any enum type without any macro or boilerplate code
* [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) - image loading library that is able to load most popular file formats
* [Dear ImGui](https://github.com/ocornut/imgui) - open source immediate mode graphic user library

## Team

* [Greenlaser](https://github.com/greeenlaser) - *Lead developer*

## Testers

* [JonesyB](https://jonesyb.com/)

## Licenses

* [GPL-3.0](LICENSE.md)
* [EULA](EULA.md)
