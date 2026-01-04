//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>

#include "KalaHeaders/log_utils.hpp"

#include "core/en_core.hpp"
#include "graphics/en_opengl_texture.hpp"
#include "gameobject/en_camera.hpp"
#include "gameobject/en_opengl_model.hpp"
#include "gameobject/en_opengl_point_light.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using ElypsoEngine::Graphics::OpenGL_Texture;
using ElypsoEngine::GameObject::Camera;
using ElypsoEngine::GameObject::OpenGL_Model;
using ElypsoEngine::GameObject::OpenGL_PointLight;

using std::to_string;

namespace ElypsoEngine::Core
{
	void EngineCore::CleanAllWindowResources(u32 windowID)
	{
		Log::Print(
			"Cleaning resources for window '" + to_string(windowID) + "'.",
			"ELYPSO_ENGINE",
			LogType::LOG_INFO);
	}
	
	void EngineCore::CleanAllResources()
	{
		Log::Print(
			"Cleaning all Elypso Engine resources.",
			"ELYPSO_ENGINE",
			LogType::LOG_INFO);
		
		Camera::registry.RemoveAllContent();
		
		OpenGL_Model::registry.RemoveAllContent();
		OpenGL_PointLight::registry.RemoveAllContent();
		OpenGL_Texture::registry.RemoveAllContent();
	}
}