//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/kg_registry.hpp"

#include "graphics/kg_texture.hpp"

namespace KalaGraphics::Import
{
    using KalaHeaders::KalaMath::vec2;

    using KalaGraphics::Core::KalaGraphicsRegistry;

    using KalaGraphics::Graphics::TexturePixelFormat;

    using std::string;
    using std::vector;
    using std::filesystem::path;
    using std::default_delete;

    struct ImportTextureData
    {
        vector<u8> pixelData{};
        vec2 size{};
        TexturePixelFormat pixelFormat = TexturePixelFormat::FORMAT_BASIC_R8G8B8A8;
    };

    class LIB_API ImportTexture
    {
    friend struct default_delete<ImportTexture>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<ImportTexture>& GetRegistry();

        //Import from path
        KNODISCARD
		static ImportTexture* Initialize(path&& texturePath);

        //Import from raw png binary data
        KNODISCARD
		static ImportTexture* Initialize(vector<u8>&& imageData);

        KNODISCARD
		u32 GetID() const;

        KNODISCARD
		const path& GetTexturePath() const;
        KNODISCARD
		const ImportTextureData& GetTextureData() const;

        void Destroy();
    private:
        ~ImportTexture();

        u32 ID{};

        path texturePath{};
        ImportTextureData textureData{};
    };
}