# Elypso engine

Elypso engine is an open source C++ OpenGL engine used in Windows. The engine has node-based scripting where the end user does not need to write a single line of code to make games with the engine. Everything is handled by the engine itself and the user only needs to know how to use the available mechanics. The engine also supports Lua package importing to modify the existing engine UI and nodes or add new nodes. The default nodes the engine comes with are also already Lua packages that are always imported by default.

[Support development on paypal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Join the discord community server](https://discord.gg/wPYCSSenX2)

[Check out the trello board](https://trello.com/b/hbt6ebCZ/elypso-engine)

[Check out the documentation](https://drive.google.com/drive/folders/17P6LjHi12q1vsdi76E6SBgLz9tZ8SwHp?usp=sharing)

### Prerequisites

```
- Windows 10/11 (untested on older versions)
- gpu with openGL 4.6 support
- Visual Studio 17 2022 (untested on older VS versions and other IDEs)
- CMake downloaded and set up with CMake/bin folder added to environment path
```

### Setup

```
- the current required path to download the repository to is C:\Documents\*\Elypso engine, you can name '*' to whatever you like. Other paths are untested and might not work.

- source code can be edited by right-clicking the Elypso engine folder and opening with visual Studio

- if you wish to compile the source manager yourself then you can use setup.bat and first Reconfigure CMake and then build which will create a new build folder and inside it is Release where the executable will be built
- follow the instructions in Source manager.exe to learn how to compile the engine yourself
```

### Logs and configuration files

```
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
* [Assimp](https://github.com/assimp/assimp) - asset importer and exporter library

## Team

* [Greenlaser](https://github.com/greeenlaser) - *Lead developer*

## Testers

* [JonesyB](https://jonesyb.com/)

## Licenses

* [GPL-3.0](LICENSE.md)
* [EULA](EULA.md)
