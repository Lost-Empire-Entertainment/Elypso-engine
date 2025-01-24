//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <unordered_map>
#include <memory>
#include <filesystem>
#include <iostream>

//external
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

//engine
#include "audio.hpp"
#include "console.hpp"
#include "core.hpp"

using std::unordered_map;
using std::unique_ptr;
using std::make_unique;
using std::move;
using std::filesystem::exists;
using std::filesystem::path;
using std::cout;

using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Engine;

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

    bool Audio::Import(const string& name)
    {
        string fullPath = (path(Engine::projectPath) / "audio" / name).string();

        if (!exists(fullPath))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot import audio file because it is not found: " + fullPath + "\n");
            return false;
        }

        if (soundMap.find(name) != soundMap.end())
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::INFO,
                "Error: Audio file already imported: " + name + "\n");
            return false;
        }

        auto sound = make_unique<ma_sound>();
        ma_result result = ma_sound_init_from_file(&engine, fullPath.c_str(), 0, NULL, NULL, sound.get());
        if (result != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to import audio file: " + name + "\n");
            return false;
        }

        soundMap[name] = move(sound);

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::INFO,
            "Successfully imported audio file: " + name + "\n");

        return true;
    }

    bool Audio::Play(const string& name)
    {
        string fullPath = (path(Engine::projectPath) / "audio" / name).string();
        cout << "name: " << name << ", full path: " << fullPath << "\n";

        if (!IsImported(name))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot play audio file because it has not been imported: " + fullPath + "\n");
            return false;
        }

        auto it = soundMap.find(name);
        if (it == soundMap.end())
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Audio file not found in sound map: " + name + "\n");
            return false;
        }

        //reset audio to the beginning
        ma_result seekResult = ma_sound_seek_to_pcm_frame(it->second.get(), 0);
        if (seekResult != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to reset audio playback position: " + name + "\n");
            return false;
        }

        ma_result result = ma_sound_start(it->second.get());
        if (result != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to play audio file: " + name + "\n");
            return false;
        }

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Playing audio file from the beginning: " + name + "\n");

        return true;
    }
    bool Audio::Stop(const string& name)
    {
        string fullPath = (path(Engine::projectPath) / "audio" / name).string();
        cout << "name: " << name << ", full path: " << fullPath << "\n";

        if (!IsImported(name))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot stop audio file because it has not been imported: " + fullPath + "\n");
            return false;
        }

        auto it = soundMap.find(name);
        if (it == soundMap.end())
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Audio file not found in sound map: " + name + "\n");
            return false;
        }

        ma_result result = ma_sound_stop(it->second.get());
        if (result != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to stop audio file: " + name + "\n");
            return false;
        }

        //reset the audio position to the start, so the next play starts fresh
        ma_result seekResult = ma_sound_seek_to_pcm_frame(it->second.get(), 0);
        if (seekResult != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to reset audio playback position after stopping: " + name + "\n");
            return false;
        }

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Stopped audio file and reset position: " + name + "\n");

        return true;
    }

    bool Audio::Pause(const string& name)
    {
        string fullPath = (path(Engine::projectPath) / "audio" / name).string();
        cout << "name: " << name << ", full path: " << fullPath << "\n";

        if (!IsImported(name))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot pause audio file because it has not been imported: " + fullPath + "\n");
            return false;
        }

        auto it = soundMap.find(name);
        if (it == soundMap.end())
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Audio file not found in sound map: " + name + "\n");
            return false;
        }

        ma_result result = ma_sound_stop(it->second.get());
        if (result != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to pause audio file: " + name + "\n");
            return false;
        }

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Paused audio file: " + name + "\n");

        return true;
    }
    bool Audio::Continue(const string& name)
    {
        string fullPath = (path(Engine::projectPath) / "audio" / name).string();
        cout << "name: " << name << ", full path: " << fullPath << "\n";

        if (!IsImported(name))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot continue playing audio file because it has not been imported: " + fullPath + "\n");
            return false;
        }

        auto it = soundMap.find(name);

        ma_result result = ma_sound_start(it->second.get());
        if (result != MA_SUCCESS)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Failed to continue playing audio file: " + name + "\n");
            return false;
        }

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Continuing playing audio file: " + name + "\n");

        return true;
    }

    bool Audio::Delete(const string& name)
    {
        string fullPath = (path(Engine::projectPath) / "audio" / name).string();
        cout << "name: " << name << ", full path: " << fullPath << "\n";

        if (!IsImported(name))
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Error: Cannot delete audio file because it has not been imported: " + name + "\n");
            return false;
        }

        auto it = soundMap.find(name);

        ma_sound_uninit(it->second.get());
        soundMap.erase(it);

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Successfully deleted audio file: " + name + "\n");

        return true;
    }

    bool Audio::IsImported(const string& name)
    {
        string fullPath = (path(Engine::projectPath) / "audio" / name).string();
        cout << "name: " << name << ", full path: " << fullPath << "\n";

        auto it = soundMap.find(name);
        return it != soundMap.end();
    }

    void Audio::Shutdown()
    {
        ma_engine_uninit(&engine);
    }
}