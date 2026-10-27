# pre-release 5 (in development)

Changes:
- added clickable UI buttons for debugging (in development)
- switched to dynamic rendering in KalaGraphics (in development)
- ...

Instructions:
- same as previous

Fixes:
- fixed stacking commands after pressing V + 1, V + 2 or V + 3
- ...

# pre-release 4

Changes:
- added texture to cube
- can toggle between filter mode

Instructions:
- same as previous except:
    - press N + 1 to toggle test texture filter mode between linear and nearest
    - press M + 1 to toggle camera movement on and off

Fixes:
- fixed windows build having popup message and title swapped
- fixed windows build crashing when minimizing
- fixed windows build notification not appearing

# pre-release 3

Changes:
- added system sound on linux
- added notifications on linux
- added file selection via zenity on linux
    - choose file types (any, executable(counts as any on linux), directory, custom)
    - can select one or multiple (multiple is unused if choosing directory as file type target)
    - can enforce root directory (file explorer can navigate outside but post-selection filter removes illegal files)
    - custom file type allows to insert list of user-selected supported file types
- drag any files onto the window to display a popup that confirms file dragging works
- added 3D camera movement and test mesh 

Instructions:
- press Z + 1 to disable vsync
- press Z + 2 to set vsync to adaptive
- press Z + 3 to set vsync to triple buffered
- press Z + 4 to toggle fps counter on and off
- press X + 1 to play 'OK' sound
- press X + 2 to play 'ERROR' sound
- press C + 1 to create a test notification
- press V + 1 to open file explorer to select a single of any file
- press V + 2 to open file explorer to select a single of any dir
- press V + 3 to open file explorer to select multiple of .md, .txt, .vert, .frag or .spv. root path is enforced and files not inside root path are filtered out after file explorer closes
- press B + 1 to toggle fullscreen on and off
- press B + 2 to toggle resizable on and off
- press N + 1 to toggle camera movement on and off
- press WASD for regular movement
- press Q to go down and E to go up
- hold left shift to sprint (double move speed)

Note: Linux version requires you to download zenity and libcanberra

# pre-release 2

- fps stabilization fixes
- vulkan backend cleanup
- draw a triangle on screen

Note: Added required build dir, must stay next to executable
Note: Linux only

# pre-release 1

- no changes, this is the first build

Note: Windows only
