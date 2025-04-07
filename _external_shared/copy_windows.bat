@echo off

echo Starting to copy Elypso engine...
echo.

set "PROJECT_ROOT=%~dp0"

:: Origin paths

set "ASSIMP_RELEASE=%PROJECT_ROOT%\Assimp\release\assimp-vc143-mt.dll"
set "ASSIMP_DEBUG=%PROJECT_ROOT%\Assimp\debug\assimp-vc143-mtd.dll"

set "GLFW_RELEASE=%PROJECT_ROOT%\GLFW\release\glfw3.dll"
set "GLFW_DEBUG=%PROJECT_ROOT%\GLFW\debug\glfw3.dll"

set "CRASH_RELEASE=%PROJECT_ROOT%\KalaCrashHandler\release\KalaCrashHandler.dll"
set "CRASH_DEBUG=%PROJECT_ROOT%\KalaCrashHandler\debug\KalaCrashHandlerD.dll"

set "PHYSICS_RELEASE=%PROJECT_ROOT%\KalaPhysics\release\KalaPhysics.dll"
set "PHYSICS_DEBUG=%PROJECT_ROOT%\KalaPhysics\debug\KalaPhysicsD.dll"

:: Target paths

set "ENGINE_RELEASE_DLLS=%PROJECT_ROOT%..\Engine\files\external dlls\release"
set "ENGINE_DEBUG_DLLS=%PROJECT_ROOT%..\Engine\files\external dlls\debug"

set "GAME_RELEASE_DLLS=%PROJECT_ROOT%..\Game\files\external dlls\release"
set "GAME_DEBUG_DLLS=%PROJECT_ROOT%..\Game\files\external dlls\debug"

:: Copy release dlls

copy /Y "%ASSIMP_RELEASE%"  "%ENGINE_RELEASE_DLLS%\"
copy /Y "%ASSIMP_RELEASE%"  "%GAME_RELEASE_DLLS%\"
copy /Y "%GLFW_RELEASE%"    "%ENGINE_RELEASE_DLLS%\"
copy /Y "%GLFW_RELEASE%"    "%GAME_RELEASE_DLLS%\"
copy /Y "%CRASH_RELEASE%"   "%ENGINE_RELEASE_DLLS%\"
copy /Y "%CRASH_RELEASE%"   "%GAME_RELEASE_DLLS%\"
copy /Y "%PHYSICS_RELEASE%" "%ENGINE_RELEASE_DLLS%\"
copy /Y "%PHYSICS_RELEASE%" "%GAME_RELEASE_DLLS%\"

:: Copy target dlls

copy /Y "%ASSIMP_DEBUG%"  "%ENGINE_DEBUG_DLLS%\"
copy /Y "%ASSIMP_DEBUG%"  "%GAME_DEBUG_DLLS%\"
copy /Y "%GLFW_DEBUG%"    "%ENGINE_DEBUG_DLLS%\"
copy /Y "%GLFW_DEBUG%"    "%GAME_DEBUG_DLLS%\"
copy /Y "%CRASH_DEBUG%"   "%ENGINE_DEBUG_DLLS%\"
copy /Y "%CRASH_DEBUG%"   "%GAME_DEBUG_DLLS%\"
copy /Y "%PHYSICS_DEBUG%" "%ENGINE_DEBUG_DLLS%\"
copy /Y "%PHYSICS_DEBUG%" "%GAME_DEBUG_DLLS%\"

echo.
echo Finished copying Elypso engine DLLs!

pause
exit /b 0