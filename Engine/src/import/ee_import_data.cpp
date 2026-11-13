//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <fstream>

#include "core_utils.hpp"
#include "log_utils.hpp"
#include "file_utils.hpp"

#include "import/ee_import_data.hpp"

using KalaHeaders::KalaCore::ContainsValue;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaFile::ReadInt;

using ElypsoEngine::Export::BINARY_MAGIC;
using ElypsoEngine::Export::BINARY_VERSION;
using ElypsoEngine::Export::BINARY_EXT;
using ElypsoEngine::Export::NAVIGATION_HEADER_SIZE;
using ElypsoEngine::Export::AssetType;
using ElypsoEngine::Export::IdentificationHeader;
using ElypsoEngine::Export::NavigationHeader;
using ElypsoEngine::Export::UserBlock;

using std::to_string;
using std::vector;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::is_regular_file;
using std::ifstream;
using std::ios;
using std::streampos;
using std::streamoff;
using std::streamsize;

struct LIB_API StreamedAssetData
{
    path edatPath{};
    IdentificationHeader idHeader{};
    vector<NavigationHeader> navHeaders{};
};

static vector<StreamedAssetData> streamedAssetData{};

namespace ElypsoEngine::Import
{
    string ImportData::ImportAssetData(const path& assetPath)
    {
        if (!exists(assetPath))
        {
            return "Failed to import asset data because asset data file '" + assetPath.string() + "' was not found!";
        }
        if (!is_regular_file(assetPath))
        {
            return "Failed to import asset data because asset data file '" + assetPath.string() + "' is not a regular file!";
        }
        if (assetPath.extension() != BINARY_EXT)
        {
            return "Failed to import asset data because asset data file '" + assetPath.string() + "' extension is not valid!";
        }

        ifstream file(assetPath, ios::binary | ios::ate);
        if (!file.is_open())
        {
            return "Failed to import asset data because asset data file '" + assetPath.string() + "' couldn't be opened!";
        }

        const streampos endPos = file.tellg();

        if (endPos < 0)
        {
            return "Failed to import asset data because asset data file '" + assetPath.string() + "' size couldn't be determined!";
        }

        const size_t fileSize = scast<size_t>(endPos);
        file.seekg(0, ios::beg);

        IdentificationHeader idHeader{};

        vector<u8> idBytes(
            sizeof(idHeader.binaryMagic)
            + sizeof(idHeader.binaryVersion)
            + sizeof(idHeader.assetCount));

        if (!file.read(
            rcast<char*>(idBytes.data()),
            idBytes.size()))
        {
            return 
                "Failed to get identification header from asset data file '" 
                + assetPath.string() + "' because it was malformed!";
        }

        memcpy(
            idHeader.binaryMagic, 
            idBytes.data(), 
            sizeof(idHeader.binaryMagic));
        if (idHeader.binaryMagic[0] != BINARY_MAGIC[0]
            || idHeader.binaryMagic[1] != BINARY_MAGIC[1]
            || idHeader.binaryMagic[2] != BINARY_MAGIC[2]
            || idHeader.binaryMagic[3] != BINARY_MAGIC[3])
        {
            return 
                "Failed to get identification header from asset data file '" 
                + assetPath.string() + "' because binary magic was invalid!";
        }

        memcpy(
            idHeader.binaryVersion,
            idBytes.data() + sizeof(idHeader.binaryMagic),
            sizeof(idHeader.binaryVersion));
        if (idHeader.binaryVersion[0] != BINARY_VERSION[0]
            || idHeader.binaryVersion[1] != BINARY_VERSION[1])
        {
            return 
                "Failed to get identification header from asset data file '" 
                + assetPath.string() + "' because binary version was unsupported!";
        }

        if (!ReadInt<u16>(
            idBytes, 
            sizeof(idHeader.binaryMagic) 
            + sizeof(idHeader.binaryVersion),
            idHeader.assetCount))
        {
            return 
                "Failed to get identification header from asset data file '" 
                + assetPath.string() + "' because asset count couldn't be read!";
        }

        const size_t navDataSize = NAVIGATION_HEADER_SIZE * idHeader.assetCount;

        if (fileSize < idBytes.size()
            || navDataSize > fileSize - idBytes.size())
        {
            return 
                "Failed to get navigation headers from asset data file '" 
                + assetPath.string() + "' because it was malformed!";
        }

        vector<u8> navBytes(navDataSize);

        if (!navBytes.empty()
            && !file.read(
                rcast<char*>(navBytes.data()),
                navBytes.size()))
        {
            return 
                "Failed to get navigation headers from asset data file '" 
                + assetPath.string() + "' because it was malformed!";
        }

        vector<NavigationHeader> navHeaders(idHeader.assetCount);

        size_t offset{};
        for (NavigationHeader& nh : navHeaders)
        {
            memcpy(
                nh.assetName,
                navBytes.data() + offset,
                sizeof(nh.assetName));
            offset += sizeof(nh.assetName);

            string nhName(rcast<const char*>(nh.assetName), sizeof(nh.assetName));
            nhName.erase(nhName.find_last_not_of(' ') + 1);

            if (!ReadInt<u8>(navBytes, offset, nh.assetType))
            {
                return 
                    "Failed to get navigation headers from asset data file '" 
                    + assetPath.string() + "' because asset '" + nhName + "' type couldn't be read!";
            }
            if (nh.assetType > scast<u8>(AssetType::A_FONT_OTF))
            {
                return 
                    "Failed to get navigation headers from asset data file '" 
                    + assetPath.string() + "' because asset '" + nhName + "' type was invalid!";
            }
            offset += sizeof(nh.assetType);

            if (!ReadInt<u64>(navBytes, offset, nh.assetOffset))
            {
                return 
                    "Failed to get navigation headers from asset data file '" 
                    + assetPath.string() + "' because asset '" + nhName + "' offset couldn't be read!";
            }
            offset += sizeof(nh.assetOffset);

            if (!ReadInt<u32>(navBytes, offset, nh.assetSize))
            {
                return 
                    "Failed to get navigation headers from asset data file '" 
                    + assetPath.string() + "' because asset '" + nhName + "' size couldn't be read!";
            }
            offset += sizeof(nh.assetSize);
        }

        const size_t dataStart = idBytes.size() + navDataSize;

        for (const NavigationHeader& nh : navHeaders)
        {
            if (nh.assetOffset < dataStart
                || nh.assetOffset > fileSize
                || nh.assetSize > fileSize - nh.assetOffset)
            {
                return "Failed to import asset data because asset data file '" + assetPath.string() + "' asset offset or size was invalid!";
            }
        }

        streamedAssetData.push_back(
            {
                .edatPath = assetPath,
                .idHeader = idHeader,
                .navHeaders = std::move(navHeaders)
            });

        Log::Print(
            "Imported headers from asset data file '" + assetPath.string() + "'!",
            "EE_IMPORT_DATA",
            LogType::LOG_SUCCESS);

        return "";
    }

    vector<AssetData> ImportData::GetAssetData()
    {
        vector<AssetData> assetData{};

        for (const StreamedAssetData& sad : streamedAssetData)
        {
            for (const NavigationHeader& nh : sad.navHeaders)
            {
                string assetName(rcast<const char*>(nh.assetName), sizeof(nh.assetName));
                assetName.erase(assetName.find_last_not_of(' ') + 1);

                assetData.push_back(
                    { 
                        .assetName = assetName,
                        .assetType = scast<AssetType>(nh.assetType)
                    });
            }
        }

        if (assetData.empty())
        {
            Log::Print(
                "Failed to get asset data because no assets have been imported!",
                "EE_IMPORT_DATA",
                LogType::LOG_WARNING);
        }

        return assetData;
    }

    string ImportData::StreamAssets(
        const vector<string>& assetNames,
        vector<AssetData>& outAssetData)
    {
        auto stream_asset = [](
            const path& assetPath,
            const size_t offset, 
            const size_t size,
            vector<u8>& data) -> string
            {
                if (!exists(assetPath))
                {
                    return "Asset data file '" + assetPath.string() + "' was not found!";
                }
                if (!is_regular_file(assetPath))
                {
                    return "Asset data file '" + assetPath.string() + "' is not a regular file!";
                }
                if (assetPath.extension() != BINARY_EXT)
                {
                    return "Asset data file '" + assetPath.string() + "' extension is not valid!";
                }

                ifstream file(assetPath, ios::binary | ios::ate);
                if (!file.is_open())
                {
                    return "Asset data file '" + assetPath.string() + "' couldn't be opened!";
                }

                const streampos endPos = file.tellg();

                if (endPos < 0)
                {
                    return "Asset data file '" + assetPath.string() + "' size couldn't be determined!";
                }

                const size_t fileSize = scast<size_t>(endPos);

                if (offset > fileSize
                    || size > fileSize - offset)
                {
                    return "Asset data file '" + assetPath.string() + "' offset or size is out of file bounds!";
                }

                vector<u8> assetData(size);

                file.seekg(scast<streamoff>(offset), ios::beg);

                if (!file.read(
                    rcast<char*>(assetData.data()),
                    scast<streamsize>(size)))
                {
                    return "Asset data file '" + assetPath.string() + "' couldn't be read!";
                }

                data = std::move(assetData);

                return "";
            };

        vector<AssetData> assetData{};

        for (const string& assetName : assetNames)
        {
            bool foundAssetName{};

            for (const StreamedAssetData& sad : streamedAssetData)
            {
                for (const NavigationHeader& nh : sad.navHeaders)
                {
                    string thisAssetName(rcast<const char*>(nh.assetName), sizeof(nh.assetName));
                    thisAssetName.erase(thisAssetName.find_last_not_of(' ') + 1);

                    if (thisAssetName != assetName) continue;

                    vector<u8> data{};
                    string err = stream_asset(
                        sad.edatPath,
                        nh.assetOffset,
                        nh.assetSize,
                        data);

                    if (!err.empty())
                    {
                        return "Failed to stream asset '" + assetName + "'! Reason: " + err;
                    }

                    assetData.push_back(
                        { 
                            .assetName = thisAssetName,
                            .assetType = scast<AssetType>(nh.assetType),
                            .assetData = std::move(data)
                        });

                    foundAssetName = true;

                    break;
                }

                if (foundAssetName) break;
            }

            if (!foundAssetName)
            {
                return "Failed to stream assets because asset '" + assetName + "' was not found!";
            }
        }

        u32 assetCount = assetData.size();

        outAssetData = std::move(assetData);

        Log::Print(
            "Streamed '" + to_string(assetCount) + "' assets!",
            "EE_IMPORT_DATA",
            LogType::LOG_SUCCESS);

        return "";
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

        Log::Print(
            "Cleared imported data!",
            "EE_IMPORT_DATA",
            LogType::LOG_SUCCESS);
    }
}