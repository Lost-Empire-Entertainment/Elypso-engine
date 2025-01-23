//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <unordered_map>
#include <memory>

//external
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

//engine
#include "audio_core.hpp"
#include "console.hpp"

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

using std::unordered_map;
using std::unique_ptr;
using std::make_unique;
using std::move;

namespace Audio
{
    static ma_engine engine;
    unordered_map<string, unique_ptr<ma_sound>> soundMap;

    AudioCore::AudioCore() {}
    AudioCore::~AudioCore()
    {
        Shutdown();
    }

    bool AudioCore::Initialize()
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

    bool AudioCore::Import(const string& path)
    {
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

    bool AudioCore::Play(const string& path)
    {
        auto it = soundMap.find(path);
        if (it == soundMap.end())
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot play audio file because it is not found: " + path + "\n");
            return false;
        }

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

    void AudioCore::Shutdown()
    {
        ma_engine_uninit(&engine);
    }
}