//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "core_utils.hpp"

#include "core/kg_registry.hpp"

namespace KalaGraphics::Import
{
    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::string;
    using std::vector;
    using std::filesystem::path;
    using std::default_delete;

    struct ImportShaderData
    {
        //TODO: add spirv-reflect gathered data here
    };

    class LIB_API ImportShader
    {
    friend struct default_delete<ImportShader>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<ImportShader>& GetRegistry();

        //Compile a raw GLSL 4.6 Vulkan shader into spirv with glslc,
        //set overwrite to true if you want to overwrite existing out path
        static void Compile(
            path&& inPath,
            path&& outPath,
            bool overwrite = false);

        KNODISCARD
		static ImportShader* Initialize(path&& shaderPath);

        KNODISCARD
		u32 GetID() const;

        KNODISCARD
		const path& GetShaderPath() const;
        KNODISCARD
		const ImportShaderData& GetShaderData() const;

        void Destroy();
    private:
        ~ImportShader();

        u32 ID{};

        path shaderPath{};
        ImportShaderData shaderData{};
    };
}