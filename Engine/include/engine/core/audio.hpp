//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

//external
#include "glm.hpp"

//engine
#include "gameobject.hpp"

namespace Core
{
	using std::string;
	using glm::vec3;
	using std::shared_ptr;

	using Graphics::Shape::GameObject;

	class Audio
	{
	public:
		static inline bool isInitialized;

		static void Initialize();

		static bool Import(const string& path, const shared_ptr<GameObject>& obj);

		/// <summary>
		/// Start audio file from the very beginning.
		/// </summary>
		static bool Play(const string& path, const shared_ptr<GameObject>& obj);
		/// <summary>
		/// Stop playing audio file and reset its position to beginning.
		/// </summary>
		static bool Stop(const string& path, const shared_ptr<GameObject>& obj);

		/// <summary>
		/// Pause playing audio file but remember its position.
		/// </summary>
		static bool Pause(const string& path, const shared_ptr<GameObject>& obj);
		/// <summary>
		/// Continue playing audio file from where it was left off.
		/// </summary>
		static bool Continue(const string& path, const shared_ptr<GameObject>& obj);

		/// <summary>
		/// Set audio player volume.
		/// </summary>
		static void SetVolume(const string& path, float currVolume, const shared_ptr<GameObject>& obj);
		/// <summary>
		/// Set audio player min range if audio player is in 3D mode.
		/// </summary>
		static void SetMinRange(const string& path, float currMinRange, const shared_ptr<GameObject>& obj);
		/// <summary>
		/// Set audio player max range if audio player is in 3D mode.
		/// </summary>
		static void SetMaxRange(const string& path, float currMaxRange, const shared_ptr<GameObject>& obj);
		/// <summary>
		/// Toggle audio player positioning, 2D is absolute, 3D is relative.
		/// </summary>
		static bool Set3DState(const string& path, bool state, const shared_ptr<GameObject>& obj);

		/// <summary>
		/// Update position where player hears audio.
		/// </summary>
		static void UpdateListenerPosition(const vec3& pos, const vec3& front, const vec3& up);
		/// <summary>
		/// Update position where audio player plays sound.
		/// </summary>
		static bool UpdatePlayerPosition(const string& name, const vec3& pos, const shared_ptr<GameObject>& obj);

		/// <summary>
		/// Delete this audio player from memory.
		/// </summary>
		static bool Delete(const string& path, const shared_ptr<GameObject>& obj);

		/// <summary>
		/// Check if this audio file has reached its end
		/// </summary>
		/// <param name="path"></param>
		static bool HasReachedEnd(const string& path, const shared_ptr<GameObject>& obj);

		/// <summary>
		/// Check if this audio file is imported to memory or not.
		/// </summary>
		static bool IsImported(const string& path, const shared_ptr<GameObject>& obj);

		static void Shutdown();
	};
}