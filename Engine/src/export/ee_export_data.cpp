//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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

using KalaHeaders::KalaString::IsStringInRange;

using KalaWindow::Core::MAX_NAME_LENGTH;

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

static constexpr u8 MAX_ASSET_NAME_LENGTH = 32;

static string VerifyAssetBlocks(const vector<UserBlock>& assetBlocks)
{
    auto is_valid_name = [](string_view n) -> bool
        {
            return
                IsStringInRange(n, 4, MAX_NAME_LENGTH)
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
        path abspath = absolute(b.blockPath).lexically_normal();

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
        size_t size) -> string
        {
            if (size == 0) return "Failed to append binary data because size was empty!";

            const size_t offset = binaryData.size();
            binaryData.resize(offset + size);

            memcpy(
                binaryData.data() + offset,
                source,
                size);

            return "";
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
            append_binary_data(
                binaryData, 
                &assetCount, 
                sizeof(assetCount));

            return "";
        };

    auto write_asset_data = [
        &append_binary_data](
        vector<u8>& binaryData,
        const vector<UserBlock>& assetBlocks) -> string
        {
            u64 dataBlockStartOffset = 
                binaryData.size() 
                + (sizeof(NavigationHeader) * assetBlocks.size());

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
                            path abspath = absolute(b.blockPath).lexically_normal();

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

            append_binary_data(
                binaryData, 
                navHeaders.data(), 
                navHeaders.size() * sizeof(NavigationHeader));

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
    bool ExportData::ExportAssetData(
        const path& targetPath,
        const vector<UserBlock>& assetBlocks,
        bool allowModify)
    {
        if (targetPath.empty())
        {
            Log::Print(
                "Failed to export to target path because no path was provided!",
                "EE_EXPORT_DATA",
                LogType::LOG_WARNING);

            return false;
        }

        path abstarget = absolute(targetPath).lexically_normal();

        if (!allowModify
            && exists(abstarget))
        {
            Log::Print(
                "Failed to export to target path '" + targetPath.string() 
                + "' because it exists but existing path modification is disabled!",
                "EE_EXPORT_DATA",
                LogType::LOG_WARNING);

            return false;
        }

        if (abstarget.extension() != BINARY_EXT)
        {
            Log::Print(
                "Failed to export to target path '" + targetPath.string() + "' because its extension was not correct!",
                "EE_EXPORT_DATA",
                LogType::LOG_WARNING);

            return false;
        }

        string err = VerifyAssetBlocks(assetBlocks);
        if (!err.empty())
        {
            Log::Print(
                "Failed to export to target path '" + targetPath.string() + "'! Reason: " + err,
                "EE_EXPORT_DATA",
                LogType::LOG_WARNING);

            return false;
        }

        vector<u8> binaryData = WriteData(abstarget, assetBlocks);
        if (binaryData.empty()) return false;

        err = WriteBinaryDataToFile(
            abstarget, 
            binaryData,
            false,
            allowModify);
        if (!err.empty())
        {
            Log::Print(
                "Failed to export to target path '" + targetPath.string() + "'! Reason: " + err,
                "EE_EXPORT_DATA",
                LogType::LOG_ERROR,
                2);

            return false;
        }

        Log::Print(
            "Finished exporting to target path '" + targetPath.string() + "'!",
            "EE_EXPORT_DATA",
            LogType::LOG_SUCCESS);

        return true;
    }
}