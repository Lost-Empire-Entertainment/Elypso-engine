//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <map>
#include <string>

//engine
#include "camera.hpp"

using glm::vec3;
using glm::mat4;
using std::map;
using std::string;

using Graphics::Camera;

namespace Graphics
{
	class Render
	{
	public:
		static inline bool waitBeforeCountsUpdate;

		static inline bool isSceneSelected;
		static inline map<int, string> aspectRatio
		{
			{0, "Unlocked"},
			{1, "16:9"},
			{2, "16:10"},
			{3, "21:9"},
			{4, "32:9"},
			{5, "4:3"},
		};
		static inline int currentIndex = 0;

		static inline vec3 backgroundColor = vec3(0.1f, 0.1f, 0.1f);

		static inline GLFWwindow* window;

		static inline mat4 projection;
		static inline mat4 view;

		static Camera camera;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void SetWindowNameAsUnsaved(bool state);
		static void WindowLoop();
		static void RenderToImguiWindow();

		static void UpdateCounts();
	private:
		static inline bool showSceneWindowDebugMenu = true;
		static inline bool showLeftCornerContent = true;
		static inline int objectsCount = 0;
		static inline int verticesCount = 0;

		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void FramebufferSetup();
		static void ContentSetup();
		static void SkyboxSetup();

		static void SceneWindowDebugMenu();
		static void SceneWindowLeftCornerContent();
		static void SceneWindowActionButtons();
	};
}