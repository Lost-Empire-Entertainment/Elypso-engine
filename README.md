# Elypso engine

Elypso engine is a game development engine made with OpenGL and C++ 20. It is currently in the prototype phase and is not yet functional enough to make games with.

The main branch holds a stable version of the engine that is up to date with the latest Release version, the indev branch is an unstable version of the next engine version and is unsafe to use in production.

Everything is still in early development and is highly likely to be modified a lot before alpha is released so there is no documentation provided at the current stage. Alpha is aimed for june 2024 and will include a bare-bones structure for making very basic games and the engine will significantly improve during alpha. Beta will release at the beginning of 2025 and is aimed to be worked on for the entirety of 2025 so the full release of this engine is aimed for the start of 2026.

---

### False positives from Windows Security

If this is your first time running the program then Windows may throw false positives like trojan or other "security threats". Follow [this Microsoft tutorial](https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26) to exclude the Hub, Engine and Game folders from Windows Security scans.

### Links

[Support development on paypal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Official website (documentation coming soon)](https://elypsoengine.com)

[Check out the trello board](https://trello.com/b/hbt6ebCZ/elypso-engine)

---

### Team

* [Greenlaser](https://github.com/greeenlaser) - *Lead developer*

---

### Licenses

* [Elypso engine propietary license](LICENSE.md)

# Setup

If you still wish to set up the hub, engine and game for testing or to see how it all runs on your end then here is the current setup.

Notes:
- You can not run Elypso Engine directly - you must create a project with Elypso Hub and run that project through the hub to use the engine.
- The game exe itself along with its required files and folders are found in 'Elypso-engine\Game\build\Release'.

0) (IF YOU ARE COMPILING FROM SOURCE)
	- 0.1) Run 'build.bat' in Hub folder
	- 0.2) Select '1' (Build Elypso Hub) - this sets up the hub for external use outside of Visual Studio or your preferred IDE.
	- 0.3) Repeat '0.2' but in 'build.bat' in the Engine folder.
1) Launch Elypso Hub
2) Click 'Set engine path'
	- 2.1) File explorer will open - go to 'Elypso-engine\Engine\build\Release' and select Elypso Engine.exe
3) Click 'New Project'
	- 3.1) File explorer will open - create a new folder anywhere outside Engine, Hub or Game folders (this will be the folder where all projects are stored inside of)
	- 3.2) open the newly created folder and create another folder inside that (this will be the project folder itself)
4) A new project is created inside Elypso Hub! Click 'Launch' to run it with Elypso Engine
5) Click 'Compile' at the top bar or press 'Left Control' + 'B'
	- 5.1) Wait for the cmd window to finish compiling, the game exe should open if no errors were encountered. If you made it this far then that means you successfully set up the engine for making games!
