//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <map>
#include <string>
#include <memory>

//external
#include "glad.h"
#include "glfw3.h"
#include "glm.hpp"

//engine
#include "gameobject.hpp"

namespace Graphics
{
	using glm::vec3;
	using glm::mat4;
	using std::map;
	using std::string;
	using std::shared_ptr;

	using Graphics::Shape::GameObject;

	class Render
	{
	public:
#if ENGINE_MODE
		/// <summary>
		/// The scene camera used by the engine
		/// </summary>
		static inline shared_ptr<GameObject> sceneCamera;
#endif
		/// <summary>
		/// The camera that is currently in use by the engine or game
		/// </summary>
		static inline shared_ptr<GameObject> activeCamera;

		static inline GLFWwindow* window;

		static inline mat4 projection;
		static inline mat4 view;

		static inline unsigned int textureColorbuffer;
		static inline unsigned int rbo;

		//default color applied to all models in the scene
		static inline vec3 globalAmbientColor = vec3(0.3f, 0.4f, 0.5f);
		//How strong the intensity of the global ambient color is
		static inline float globalAmbientIntensity = 0.5f;

		static void RenderSetup();
		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);
		static void SetWindowNameAsUnsaved(bool state);
		static void WindowLoop();
	private:
		static void GLFWSetup();
		static void WindowSetup();
		static void GladSetup();
		static void ContentSetup();

#if ENGINE_MODE
		/// <summary>
		/// Initializes scene camera for engine at the first frame of the render loop
		/// </summary>
		static void InitializeSceneCamera();

		static void FramebufferSetup();
#else
		/// <summary>
		/// Assigns game camera for game at the first frame of the render loop
		/// </summary>
		static void AssignGameCamera();
#endif
	};
}