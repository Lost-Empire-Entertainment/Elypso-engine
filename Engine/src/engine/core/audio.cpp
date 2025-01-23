//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <unordered_map>
#include <memory>
#include <filesystem>

//external
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

//engine
#include "audio.hpp"
#include "console.hpp"

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

using std::unordered_map;
using std::unique_ptr;
using std::make_unique;
using std::move;
using std::filesystem::exists;

namespace Core
{
    static ma_engine engine;
    unordered_map<string, unique_ptr<ma_sound>> soundMap;

    bool Audio::Initialize()
    {
        ma_result result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::INITIALIZE,
                Type::EXCEPTION,
                "Error: Failed to initialize miniaudio!\n");
            return false;
        }

        ConsoleManager::WriteConsoleMessage(
            Caller::INITIALIZE,
            Type::DEBUG,
            "Miniaudio initialized successfully!\n");

        return true;
    }

    bool Audio::Import(const string& path)
    {
        if (!exists(path))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot import audio file because it is not found: " + path + "\n");
            return false;
        }

        if (soundMap.find(path) != soundMap.end())
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::INFO,
                "Error: Audio file already imported: " + path + "\n");
            return false;
        }

        auto sound = make_unique<ma_sound>();
        ma_result result = ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, sound.get());
        if (result != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to import audio file: " + path + "\n");
            return false;
        }

        soundMap[path] = move(sound);

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Successfully imported audio file: " + path + "\n");

        return true;
    }

    bool Audio::Play(const string& path)
    {
        if (!IsImported(path))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot play audio file because it has not been imported: " + path + "\n");
            return false;
        }

        auto it = soundMap.find(path);

        ma_result result = ma_sound_start(it->second.get());
        if (result != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to play audio file: " + path + "\n");
            return false;
        }

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Playing audio file: " + path + "\n");

        return true;
    }

    bool Audio::Delete(const string& path)
    {
        if (!IsImported(path))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot delete audio file because it has not been imported: " + path + "\n");
            return false;
        }

        auto it = soundMap.find(path);

        ma_sound_uninit(it->second.get());
        soundMap.erase(it);

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Successfully deleted audio file: " + path + "\n");

        return true;
    }

    bool Audio::IsImported(const string& path)
    {
        auto it = soundMap.find(path);
        return it != soundMap.end();
    }

    void Audio::Shutdown()
    {
        ma_engine_uninit(&engine);
    }
}