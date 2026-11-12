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

    static constexpr string_view BINARY_MAGIC = "ESCN";
    static constexpr string_view BINARY_VERSION = "01";
    static constexpr string_view BINARY_EXT = ".escn";

    //Identification header at the top of the binary
    struct LIB_API IdentificationHeader
    {
        u8 binaryMagic[4] = 
        { 
            BINARY_MAGIC[0], 
            BINARY_MAGIC[1], 
            BINARY_MAGIC[2], 
            BINARY_MAGIC[3]
        };
        u8 binaryVersion[2] = { BINARY_VERSION[0], BINARY_VERSION[1] };
        u16 entityCount{};
    };

    //Data per data block for each field in an entity in the escn file
    struct LIB_API KeyValuePair
    {
        u8 key[32] = {};
        u8 value[128] = {};
    };

    //Container for one entity
    struct LIB_API EntityData
    {
        u8 entityName[32] = {};
        u8 entityType{};
        u64 entityOffset{};
        array<KeyValuePair, 128> entityData{};
    };

    class LIB_API ExportScene
    {
    public:
        
    };
}