# Elypso engine

**Elypso Engine** is a game development engine that is made with OpenGL, C++ 20 and [several libraries](https://github.com/Lost-Empire-Entertainment/Elypso-engine/blob/main/LIBRARIES.md) and it is primarily aimed for Windows 10 and 11. Linux support is coming very soon and this README file will be updated accordingly once Linux support is confirmed to be working.

The engine is currently in the Alpha stage and has finished all of its Prototype versions.
- Elypso Engine beta release date is aimed for spring 2025.
- Elypso Engine full release date is aimed for spring 2026.

These dates are not final and may change so check back here regularly to see updates.

The documentation for **Elypso Hub**, **Elypso Engine**, **Game Template** and the **Compiler** will be found at the [website](https://elypsoengine.com) so please look there for information, this README file will not cover how to install or use them.

---

### Prerequisites

To compile **Elypso Hub**, **Elypso Engine**, **Game Template**, or the **Compiler** from their source code, it is recommended to have the following tools installed:

- **CMake**
- **Visual Studio 2022 Community Edition** (or another edition of Visual Studio 2022)

If you prefer to compile with a different setup, you will need to provide your own compiler and IDE. By default, the engine compiles the game using **CMake** and **MSVC** (Visual Studio 2022). If you choose to use another IDE or compiler, you will need to modify the `RunInstaller` function in the game compiler script, located at: `Engine/src/engine/core/compile.cpp`.

For those who want to use **CMake** and **Visual Studio 2022**, detailed setup instructions are provided:

- **CMake Setup**: [Download 'Windows x64 installer here' here](https://cmake.org/download/). Setup instructions are available in `_external_shared/CMake/setup.txt`.
- **Visual Studio Setup**: [Download preferred version here](https://visualstudio.microsoft.com/downloads/). Setup instructions are available in `_external_shared/Visual_Studio/setup.txt`.

---

### False positives from Windows Security

If this is your first time running the program then Windows may throw false positives like trojan or other "security threats". Follow [this Microsoft tutorial](https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26) to exclude **Elypso Hub**, **Elypso Engine**, **Game Template** and the **Compiler** folders from Windows Security scans.

---

### Reporting a security vulnerability

Lost Empire Entertainment accepts all security vulnerability concerns or discovered security bugs at sanderveski@gmail.com, do not post them as Issues.

---

### Discord Game SDK and Rich Presence

By default **Elypso Engine** and its **Game Template** will not include **Discord Game SDK** because Discord does not allow to distribute it, if you wish to use it in your game or in the engine then follow the instructions in `_external_shared/Discord/setup.txt`.

---

### Links

[Support development on paypal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Official discord server](https://discord.gg/FqJgy2SvDs)

[Official youtube channel](https://youtube.com/greenlaser)

[Official website (documentation coming soon)](https://elypsoengine.com)

[Track development on the Trello board](https://trello.com/b/hbt6ebCZ/elypso-engine)

---

### Team

* [Greenlaser](https://github.com/greeenlaser) - *Lead developer*

* [tkpmonke](https://github.com/tkpmonke) - *Tester*

---

### Licenses

* [Elypso engine propietary license](LICENSE.md)
