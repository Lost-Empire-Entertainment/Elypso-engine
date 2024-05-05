# Elypso engine

Elypso engine is a game development engine made with OpenGL and C++ 20. It is currently in the prototype phase and is not yet functional enough to make games with.

The main branch holds a stable version of the engine that is up to date with the latest Release version, the indev branch is an unstable version of the next engine version and is unsafe to use in production.

Everything is still in early development and is highly likely to be modified a lot before alpha is released so there is no documentation provided at the current stage. Alpha is aimed for june 2024 and will include a bare-bones structure for making very basic games and the engine will significantly improve during alpha. Beta will release at the beginning of 2025 and is aimed to be worked on for the entirety of 2025 so the full release of this engine is aimed for the start of 2026.

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
	- 2.1) File explorer will open - find and select Elypso Hub.exe
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

# Links

[Support development on paypal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Join the discord community server](https://discord.gg/wPYCSSenX2)

[Check out the trello board](https://trello.com/b/hbt6ebCZ/elypso-engine)

# Team

* [Greenlaser](https://github.com/greeenlaser) - *Lead developer*

# Licenses

* [Elypso engine propietary license](LICENSE.md)
