//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <filesystem>
#include <functional>
#include <source_location>

#include "core_utils.hpp"

namespace KalaAudio::Core
{
	using std::string;
	using std::string_view;
	using std::filesystem::path;
	using std::function;
	using std::source_location;
	
	class LIB_API KalaAudioCore
	{
	public:
		KNODISCARD
		static u32 GetGlobalID();
		static void SetGlobalID(u32 newID);
		
        KNODISCARD
        static path GetExePath();

        static void SetExternalHandler(function<void(string, string, const source_location&)>&& newExternalHandler);

        KNORETURN
        static void ForceClose(
            string_view title,
            string_view reason,
            const source_location& location = source_location::current());
	};
}