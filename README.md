Welcome to the main repository of Elypso engine which is an open source C++ OpenGL engine primarily focusing on RPG and shooter games and will be the main game creation engine for my game studio Lost Empire Entertainment once it is production ready.

Join the community server: https://discord.com/invite/3jxDdHJ6SQ

Check out the trello board: https://trello.com/b/hbt6ebCZ/elypso-engine

Libraries used in this engine:
- CMake (alternative to visual studio for generating build files for the engine)
- GLFW (provides a simple API for creating windows, contexts and surfaces, receiving input and events)
- GLAD (simplifies the process of managing OpenGL function pointers. It generates platform-specific code for loading OpenGL functions, making it easier to use OpenGL in a cross-platform manner)
- GLM (C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specification)
- GLTF (supports static models, animation, and moving scenes)
- Dear ImGUI (bloat-free graphical user interface library for C++. It outputs optimized vertex buffers that you can render anytime in your 3D-pipeline-enabled applications)

Cross-platform executable libraries:
- windows executable: visual studio, cmake, cpack
- linux executable: visual studio, cmake, cpack
- mac executable: visual studio, cmake, cpack

Cross-platform installer libraries:
- windows installer: nsis
- linux installer: dpkg (creates .deb file)
- mac installer: create-dmg (creates .dmg file)
