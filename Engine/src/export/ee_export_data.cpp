//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core_utils.hpp"
#include "log_utils.hpp"
#include "file_utils.hpp"
#include "string_utils.hpp"

#include "core/kw_core.hpp"

#include "export/ee_export_data.hpp"

using KalaHeaders::KalaCore::ContainsValue;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaFile::WriteBinaryDataToFile;
using KalaHeaders::KalaFile::ReadBinaryDataFromFile;
using KalaHeaders::KalaFile::HasAnyUnsafePathChar;
using KalaHeaders::KalaFile::GetCleanPath;
using KalaHeaders::KalaFile::WriteInt;

using KalaHeaders::KalaString::IsStringInRange;

using KalaWindow::Core::MAX_NAME_LENGTH;
using KalaWindow::Core::KalaWindowCore;

using ElypsoEngine::Export::MAX_ASSET_NAME_LENGTH;
using ElypsoEngine::Export::NAVIGATION_HEADER_SIZE;
using ElypsoEngine::Export::BINARY_MAGIC;
using ElypsoEngine::Export::BINARY_VERSION;
using ElypsoEngine::Export::SUPPORTED_EXTENSIONS;
using ElypsoEngine::Export::AssetType;
using ElypsoEngine::Export::NavigationHeader;
using ElypsoEngine::Export::UserBlock;

using std::string;
using std::string_view;
using std::fill_n;
using std::vector;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::is_regular_file;

static string VerifyAssetBlocks(const vector<UserBlock>& assetBlocks)
{
    auto is_valid_name = [](string_view n) -> bool
        {
            bool outResult{};
            string err = IsStringInRange(
                n, 
                4, 
                MAX_NAME_LENGTH,
                outResult);

            if (!err.empty())
            {
                Log::Print(
                    "Failed to check asset block name! Reason: " + err,
                    "EE_EXPORT_DATA",
                    LogType::LOG_WARNING);

                return false;
            }

            return
                outResult
                && !HasAnyUnsafePathChar(n, true);
        };

    auto is_valid_path = [](const path& p) -> bool
        {
            return
                exists(p)
                && is_regular_file(p)
                && p.has_extension() 
                && ContainsValue(SUPPORTED_EXTENSIONS, p.extension());
        };

    vector<string> foundNames{};
    vector<path> foundPaths{};

    for (const UserBlock& b : assetBlocks)
    {
        path abspath = GetCleanPath(b.blockPath);

        if (!is_valid_name(b.blockName))
        {
            return "Failed to verify asset '" + b.blockName + "' because its name was invalid!";
        }
        if (!is_valid_path(abspath))
        {
            return "Failed to verify asset '" + b.blockName + "' because its path was invalid!";
        }

        if (ContainsValue(foundNames, b.blockName))
        {
            return 
                "Failed to verify asset '" + b.blockName + "' because "
                "another one with the same name has already been added!";
        }
        if (ContainsValue(foundPaths, abspath))
        {
            return 
                "Failed to verify asset '" + b.blockName + "' because "
                "another one with the same path has already been added!";
        }

        foundNames.push_back(b.blockName);
        foundPaths.push_back(abspath);
    }

    return "";
}

static vector<u8> WriteData(
    const path& targetPath,
    const vector<UserBlock>& assetBlocks)
{
    auto append_binary_data = [](
        vector<u8>& binaryData,
        const void* source,
        size_t size) -> void
        {
            if (size == 0)
            {
                KalaWindowCore::ForceClose(
                    "Elypso Engine export data error",
                    "Failed to append binary data because size was empty!");
            }

            const size_t offset = binaryData.size();
            binaryData.resize(offset + size);

            memcpy(
                binaryData.data() + offset,
                source,
                size);
        };

    vector<u8> binaryData{};

    auto write_id_header_data = [&append_binary_data](
        vector<u8>& binaryData,
        const vector<UserBlock>& assetBlocks) -> string
        {
            append_binary_data(
                binaryData, 
                BINARY_MAGIC.data(), 
                BINARY_MAGIC.size());

            append_binary_data(
                binaryData, 
                BINARY_VERSION.data(), 
                BINARY_VERSION.size());

            if (assetBlocks.size() > UINT16_MAX)
            {
                return "Asset count exceeded max allowed count!";
            }

            u16 assetCount = scast<u16>(assetBlocks.size());
            if (!WriteInt<u16>(
                binaryData,
                scast<size_t>(-1),
                assetCount))
            {
                return "Failed to serialize asset count!";
            }

            return "";
        };

    auto write_asset_data = [
        &append_binary_data](
        vector<u8>& binaryData,
        const vector<UserBlock>& assetBlocks) -> string
        {
            u64 dataBlockStartOffset = 
                binaryData.size() 
                + (NAVIGATION_HEADER_SIZE * assetBlocks.size());

            u64 accumulatedOffset{};

            vector<NavigationHeader> navHeaders{};
            vector<u8> dataBlock{};

            auto write_nav_header_and_asset_data = [
                &append_binary_data,
                &accumulatedOffset,
                &dataBlock,
                &dataBlockStartOffset,
                &navHeaders](const UserBlock& b) -> string
                {
                    auto write_data = [
                        &append_binary_data,
                        &dataBlock](
                        const UserBlock& b,
                        u32& outAssetSize,
                        u8& outAssetType) -> string
                        {
                            path abspath = GetCleanPath(b.blockPath);

                            vector<u8> outData{};

                            string err = ReadBinaryDataFromFile(abspath, outData);
                            if (!err.empty()) return "Failed to read binary '" + b.blockName + "'! Reason: " + err;

                            if (outData.size() > UINT32_MAX)
                            {
                                return "Failed to append asset '" + b.blockName + "' because it exceeded the size limit!";
                            }

                            outAssetSize = outData.size();

                            const path ext = abspath.extension();
                            if (ext == ".glb") outAssetType = scast<u8>(AssetType::A_MODEL);
                            if (ext == ".spv") outAssetType = scast<u8>(AssetType::A_SHADER);
                            if (ext == ".png") outAssetType = scast<u8>(AssetType::A_TEXTURE);
                            if (ext == ".opus") outAssetType = scast<u8>(AssetType::A_AUDIO_OPUS);
                            if (ext == ".mp3") outAssetType = scast<u8>(AssetType::A_AUDIO_MP3);
                            if (ext == ".wav") outAssetType = scast<u8>(AssetType::A_AUDIO_WAV);
                            if (ext == ".ttf") outAssetType = scast<u8>(AssetType::A_FONT_TTF);
                            if (ext == ".otf") outAssetType = scast<u8>(AssetType::A_FONT_OTF);

                            append_binary_data(
                                dataBlock,
                                outData.data(),
                                outData.size());

                            return "";
                        };

                    NavigationHeader nh{};

                    //fill all with spaces
                    fill_n(nh.assetName, MAX_ASSET_NAME_LENGTH, ' ');
                    //copy user-provided asset name at the start
                    memcpy(
                        nh.assetName,
                        b.blockName.data(),
                        b.blockName.size());

                    u32 outAssetSize{};
                    u8 outAssetType{};
                    string err = write_data(b, outAssetSize, outAssetType);
                    if (!err.empty()) return err;

                    nh.assetType = outAssetType;
                    nh.assetOffset = dataBlockStartOffset + accumulatedOffset;
                    nh.assetSize = outAssetSize;

                    accumulatedOffset += outAssetSize;

                    navHeaders.push_back(std::move(nh));

                    return "";
                };

            for (const UserBlock& b : assetBlocks)
            {
                string err = write_nav_header_and_asset_data(b);
                if (!err.empty()) return err;
            }

            for (const NavigationHeader& nh : navHeaders)
            {
                append_binary_data(
                    binaryData, 
                    nh.assetName, 
                    sizeof(nh.assetName));

                if (!WriteInt<u8>(binaryData, scast<size_t>(-1), nh.assetType)
                    || !WriteInt<u64>(binaryData, scast<size_t>(-1), nh.assetOffset)
                    || !WriteInt<u32>(binaryData, scast<size_t>(-1), nh.assetSize))
                {
                    return "Failed to serialize asset navigation header!";
                }
            }

            append_binary_data(
                binaryData,
                dataBlock.data(),
                dataBlock.size());

            return "";
        };

    string err = write_id_header_data(binaryData, assetBlocks);
    if (!err.empty())
    {
        Log::Print(
            "Failed to write to target path '" + targetPath.string() + "'! Reason: " + err,
            "EE_EXPORT_DATA",
            LogType::LOG_WARNING);

        return {};
    }

    err = write_asset_data(binaryData, assetBlocks);
    if (!err.empty())
    {
        Log::Print(
            "Failed to write to target path '" + targetPath.string() + "'! Reason: " + err,
            "EE_EXPORT_DATA",
            LogType::LOG_WARNING);

        return {};
    }

    return binaryData;
}

namespace ElypsoEngine::Export
{
    string ExportData::ExportAssetData(
        const path& targetPath,
        const vector<UserBlock>& assetBlocks,
        bool allowModify)
    {
        if (targetPath.empty()) return "Failed to export to target path because no path was provided!";

        path abstarget = GetCleanPath(targetPath);

        if (!allowModify
            && exists(abstarget))
        {
            return 
                "Failed to export to target path '" + targetPath.string() 
                + "' because it exists but existing path modification is disabled!";
        }

        if (abstarget.extension() != BINARY_EXT)
        {
            return 
                "Failed to export to target path '" + targetPath.string() 
                + "' because its extension was not correct!";
        }

        string err = VerifyAssetBlocks(assetBlocks);
        if (!err.empty())
        {
            return "Failed to export to target path '" + targetPath.string() + "'! Reason: " + err;
        }

        vector<u8> binaryData = WriteData(abstarget, assetBlocks);
        if (binaryData.empty()) return "Failed to export to target path because WriteData returned empty vector!";

        err = WriteBinaryDataToFile(
            abstarget, 
            binaryData,
            false,
            allowModify);
        if (!err.empty())
        {
            return "Failed to export to target path '" + targetPath.string() + "'! Reason: " + err;
        }

        Log::Print(
            "Finished exporting to target path '" + targetPath.string() + "'!",
            "EE_EXPORT_DATA",
            LogType::LOG_SUCCESS);

        return "";
    }
}