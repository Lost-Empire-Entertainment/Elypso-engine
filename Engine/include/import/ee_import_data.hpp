//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "core_utils.hpp"

#include "export/ee_export_data.hpp"

namespace ElypsoEngine::Import
{
    using ElypsoEngine::Export::AssetType;

    using std::string;
    using std::vector;
    using std::filesystem::path;

    struct LIB_API AssetData
    {
        vector<u8> assetData{};
        AssetType assetType{};
    };

    class LIB_API ImportData
    {
    public:
        //Import the necessary data of an edat binary into memory for runtime asset streaming
        static bool ImportAssetData(const path& assetPath);

        //Returns a vector of asset names from the currently imported edat binaries
        static vector<string> GetAssetNames();
        //Takes in known asset names, returns asset bytes and type of each found asset, fails on first not found asset
        static bool StreamAssets(
            const vector<string>& assetNames,
            vector<AssetData>& outAssetIDs);

        //Clears the list of all currently imported assets,
        //does not need to be ran on program shutdown
        static void ClearImportedData();
    };
}