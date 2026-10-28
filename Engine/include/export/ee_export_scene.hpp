//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <array>

#include "core_utils.hpp"

namespace ElypsoEngine::Export
{
    using std::string;
    using std::string_view;
    using std::filesystem::path;
    using std::vector;
    using std::array;

    constexpr string_view BINARY_NAME = "ESCN";
    constexpr string_view BINARY_VERSION = "01";
    constexpr string_view BINARY_EXT = ".escn";

    class LIB_API ExportScene
    {
    public:
        
    };
}