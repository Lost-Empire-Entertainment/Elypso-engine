//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#if defined(__clang__)
    #define KG_VK_MEM_ALLOC_IGNORE_PUSH \
        _Pragma("clang diagnostic push") \
        _Pragma("clang diagnostic ignored \"-Wnullability-completeness\"") \
        _Pragma("clang diagnostic ignored \"-Wunused-parameter\"") \
        _Pragma("clang diagnostic ignored \"-Wunused-variable\"") \
        _Pragma("clang diagnostic ignored \"-Wunused-private-field\"") \
        _Pragma("clang diagnostic ignored \"-Wmissing-field-initializers\"")
    #define KG_VK_MEM_ALLOC_IGNORE_POP \
        _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
    #define KG_VK_MEM_ALLOC_IGNORE_PUSH \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
        _Pragma("GCC diagnostic ignored \"-Wunused-variable\"") \
        _Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")
    #define KG_VK_MEM_ALLOC_IGNORE_POP \
        _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
    #define KG_VK_MEM_ALLOC_IGNORE_PUSH \
        __pragma(warning(push))
    #define KG_VK_MEM_ALLOC_IGNORE_POP \
        __pragma(warning(pop))
#else
    #error "UNKNOWN COMPILER, MUST DEFINE NULLABILITY_IGNORE_START AND NULLABILITY_IGNORE_END"
#endif

#include <string>
#include <functional>
#include <vector>
#include <filesystem>

#include "core_utils.hpp"

namespace KalaGraphics::Core
{
    using std::string;
    using std::string_view;
    using std::function;
    using std::vector;
    using std::filesystem::path;

    static constexpr u8 MIN_NAME_LENGTH = 4;
    static constexpr u8 MAX_NAME_LENGTH = 64;

    class LIB_API KalaGraphicsCore
    {
    public:
        static u32 GetGlobalID();
		static void SetGlobalID(u32 newID);

		KNODISCARD
		static path GetExePath();

        KNODISCARD
        static f64 GetDeltaTime();
        static void SetDeltaTime(f64 newValue);

        //External handler for force close, overrides local version so external version can do its own action
        static void SetExternalHandler(function<void(string, string)>&& externalHandler);

        //Force-closes the application and gives a breakpoint, good for hard stops or bad user errors,
        //assigning a callback via SetExternalHandler will always use whatever is assigned there,
        //if the callback is unassigned or invalid then it falls back to the local setup
		KNORETURN
        static void ForceClose(
			string_view title,
			string_view reason);
    };
}