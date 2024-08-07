# Elypso engine

Elypso Engine is a game development engine that is made with OpenGL, C++ 20 and [several libraries](https://github.com/Lost-Empire-Entertainment/Elypso-engine/blob/main/LIBRARIES.md) and it is primarily aimed for Windows 10 and 11. Support for Linux is planned to be added in Alpha.

- Elypso Engine alpha release date is aimed for the end of summer 2024.
- Elypso Engine beta release date is aimed for spring 2025.
- Elypso Engine full release date is aimed for spring 2026.

These dates are not final and may change so check back here regularly to see updates.

There is no documentation planned to be added until Alpha is released because the engine is currently in the Prototype phase and will change a lot during development. 

You can check out the Setup section at the bottom of this Readme file if you want to know how to try out the engine at the current stage. There is also the [Trello board](https://trello.com/b/hbt6ebCZ/elypso-engine) that shows all changes added in each build.

---

### Reporting a security vulnerability

If you believe you have found a security vulnerability that may affect your computer negatively in any way then please follow the [Security Policy file](https://github.com/Lost-Empire-Entertainment/Elypso-engine/security/policy).

---

### False positives from Windows Security

If this is your first time running the program then Windows may throw false positives like trojan or other "security threats". Follow [this Microsoft tutorial](https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26) to exclude the Hub, Engine and Game folders from Windows Security scans.

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

---

### Licenses

* [Elypso engine propietary license](LICENSE.md)

# Setup

If you still wish to set up the hub, engine and game for testing or to see how it all runs on your end then here is the current setup.

Notes:
- You can not run elypso engine directly - you must create a project with elypso hub and run that project through elypso hub to use elypso engine
- Running build bat files is currently limited only to the C drive so you should NOT place any engine, hub or game build bat files outside of C drive folders
- The game exe itself along with its required files and folders are found in Game\build\Release

0) (IF YOU ARE COMPILING FROM SOURCE)
	- 0.1) Run 'build.bat' in Hub folder
	- 0.2) Select '1' (Reconfigure CMake) - this sets up the hub for external use outside of Visual Studio or your preferred IDE. '2' (Build Elypso Hub) is ran automatically after 1 is done.
	- 0.3) Repeat '0.2' but in 'build.bat' in the Engine folder.
	- 0.4) Everything past here is same for both compiling from source and running through Github releases download.
1) Launch Elypso Hub
2) Click 'Set engine path'
	- 2.1) File explorer will open - find and select Elypso Engine.exe
3) Click 'New Project'
	- 3.1) File explorer will open - create a new folder anywhere (this will be the folder where all projects are stored inside of)
	- 3.2) open the newly created folder and create another folder inside that (this will be the project folder itself)
5) A new project is created inside Elypso Hub! Click 'Launch' to run it with Elypso Engine
6) Select 'Debug menu' tab next to Inspector
7) Select 'Debug interactions' tab
8) Click 'Game path'
	- 7.1) File explorer will open - find and select the Game folder (same directory where Engine and Hub are at)
8) Click 'Compile' at the top bar
	- 8.1) Wait for the cmd window to finish compiling, the game exe should open if no errors were encountered. If you made it this far then that means you successfully set up the engine for making games!
