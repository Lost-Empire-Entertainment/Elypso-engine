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

    constexpr string_view BINARY_MAGIC = "EDAT";
    constexpr string_view BINARY_VERSION = "01";
    constexpr string_view BINARY_EXT = ".edat";

    constexpr array<string_view, 8> SUPPORTED_EXTENSIONS = 
    {
        ".glb",
        ".spv",
        ".png",
        ".opus",
        ".mp3",
        ".wav",
        ".ttf",
        ".otf"
    };

    enum class AssetType : u8
    {
        A_MODEL = 0,
        A_SHADER = 1,
        A_TEXTURE = 2,
        A_AUDIO_OPUS = 3,
        A_AUDIO_MP3 = 4,
        A_AUDIO_WAV = 5,
        A_FONT_TTF = 6,
        A_FONT_OTF = 7
    };

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
        u16 assetCount{};
    };

    //Single navigation header
    struct LIB_API NavigationHeader
    {
        u8 assetName[32] = {};
        u8 assetType{};
        u64 assetOffset{};
        u32 assetSize{};
    };

    //Info the user gives that is used during export
    struct LIB_API UserBlock
    {
        string blockName{}; //name for this asset block to identify it during import
        path blockPath{};   //asset path before export, only used during export, unused during import
    };

    class LIB_API ExportData
    {
    public:
        //Export selected assets to target path, extension must be '.edat'.
        //Set allowModify to true to modify target path if it already exists.
        //Place assets in any order, exporter will then group by asset type and sort alphabetically.
        static bool ExportAssetData(
            const path& targetPath,
            const vector<UserBlock>& assetBlocks,
            bool allowModify = false);
    };
}