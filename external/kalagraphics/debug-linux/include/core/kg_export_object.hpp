//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <filesystem>
#include <vector>
#include <string>

#include "core_utils.hpp"

namespace KalaGraphics::Core
{
    using std::filesystem::path;
    using std::vector;
    using std::string;

    class LIB_API Export
    {
    public:
        //Exports selected texture to target path
        static void ExportTexture(
            u32 textureID,
            const path& exportPath,
            bool useCompression = true,
            bool overwrite = false);

        //Exports one or more meshes to a new .glb file
        static void ExportMeshes(
            const vector<u32>& meshIDs,
            const path& exportPath,
            bool overwrite = false);

        //Returns the json data from meshes without exporting a glb file
        static string GetMeshJsonData(const vector<u32>& meshIDs);
    };
}