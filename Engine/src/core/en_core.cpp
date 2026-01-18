//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <csignal>

#include "KalaHeaders/log_utils.hpp"

#include "core/en_core.hpp"
#include "graphics/en_opengl_texture.hpp"
#include "gameobject/en_camera.hpp"
#include "gameobject/en_opengl_model.hpp"
#include "gameobject/en_opengl_point_light.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;
using KalaHeaders::KalaLog::DateFormat;

using ElypsoEngine::Graphics::OpenGL_Texture;
using ElypsoEngine::GameObject::Camera;
using ElypsoEngine::GameObject::OpenGL_Model;
using ElypsoEngine::GameObject::OpenGL_PointLight;

using std::to_string;
using std::raise;

namespace ElypsoEngine::Core
{
	//The ID that is bumped by every object in KalaWindow when it needs a new ID
	static u32 globalID{};

	void EngineCore::SetGlobalID(u32 newID) { globalID = newID; }
	u32 EngineCore::GetGlobalID() { return globalID; }

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
		
		Camera::GetRegistry().RemoveAllContent();
		
		OpenGL_Model::GetRegistry().RemoveAllContent();
		OpenGL_PointLight::GetRegistry().RemoveAllContent();
		OpenGL_Texture::GetRegistry().RemoveAllContent();
	}

	void EngineCore::ForceClose(
		const string& target,
		const string& reason)
	{
		Log::Print(
			"\n================"
			"\nFORCE CLOSE"
			"\n================\n",
			true);

		Log::Print(
			reason,
			target,
			LogType::LOG_ERROR,
			2,
			true,
			TimeFormat::TIME_NONE,
			DateFormat::DATE_NONE);

#ifdef _WIN32
		__debugbreak();
#else
		raise(SIGTRAP);
#endif
	}
}