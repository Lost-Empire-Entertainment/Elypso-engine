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
        string assetName{};
        AssetType assetType{};
        vector<u8> assetData{};
    };

    class LIB_API ImportData
    {
    public:
        //Import the necessary data of an edat binary into memory for runtime asset streaming,
        //you can import multiple edat files over and over to stream them as needed,
        //returns string on import error
        static string ImportAssetData(const path& assetPath);

        //Returns all imported edat asset names and their types but leaves asset data empty
        static vector<AssetData> GetAssetData();
        //Takes in known asset names, returns asset name, type and data for all assets,
        //returns string on stream error
        static string StreamAssets(
            const vector<string>& assetNames,
            vector<AssetData>& outAssetData);

        //Clears the list of all currently imported assets,
        //does not need to be ran on program shutdown
        static void ClearImportedData();
    };
}