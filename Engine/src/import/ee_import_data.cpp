//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

#include "import/ee_import_data.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using ElypsoEngine::Export::IdentificationHeader;
using ElypsoEngine::Export::NavigationHeader;

using std::vector;
using std::filesystem::path;

struct LIB_API StreamedAssetData
{
    path edatPath{};
    IdentificationHeader idHeader{};
    vector<NavigationHeader> navHeaders{};
};

static vector<StreamedAssetData> streamedAssetData{};

namespace ElypsoEngine::Import
{
    bool ImportData::ImportAssetData(const path& assetPath)
    {

    }

    vector<string> ImportData::GetAssetNames()
    {

    }

    bool ImportData::StreamAssets(
        const vector<string>& assetNames,
        vector<AssetData>& outAssetIDs)
    {

    }

    void ImportData::ClearImportedData()
    {
        if (streamedAssetData.empty())
        {
            Log::Print(
                "Failed to clear imported data because there was nothing to clear!",
                "EE_IMPORT_DATA",
                LogType::LOG_WARNING);

            return;
        }

        streamedAssetData.clear();
    }
}