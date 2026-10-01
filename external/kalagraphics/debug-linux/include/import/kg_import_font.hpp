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

struct hb_blob_t;
using hb_blob = hb_blob_t*;

struct hb_face_t;
using hb_face = hb_face_t*;

struct hb_font_t;
using hb_font = hb_font_t*;

struct hb_set_t;
using hb_set = hb_set_t*;

struct hb_draw_funcs_t;
using hb_draw_funcs = hb_draw_funcs_t*;

namespace KalaGraphics::Import
{
    using KalaGraphics::Core::KalaGraphicsRegistry;

    using KalaHeaders::KalaMath::vec2;

    using std::string;
    using std::vector;
    using std::filesystem::path;
    using std::default_delete;

    struct GlyphData
    {
        u32 codepoint{};
        u32 glyphIndex{};

        i32 advance{};

        vec2 bearing{}; //used as i32
        vec2 size{}; //used as i32

        //used as u32, helps locate the top-left corner offset of the glyph in the atlas
        vec2 atlasPosition{};
    };

    struct FontData
    {
        u8 fontScale{}; //font EM value

        i32 ascender{};
        i32 descender{};
        i32 lineGap{};

        vector<GlyphData> glyphs{};

        vector<u8> atlasPixels{};
        vec2 atlasSize{};
    };

    //TODO: evolve font logic from "coverage" to signed distance field, potentially for the whole 2D mesh framework?
    
    class LIB_API ImportFont
    {
    friend struct default_delete<ImportFont>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<ImportFont>& GetRegistry();

        //Returns an individual glyphs data
        KNODISCARD
        static GlyphData* GetGlyphData(
            FontData& fontData,
            u32 codepoint);

        //Import a font from the given path, accepts .ttf and .otf,
        //sets the font's EM value to fontScale
        KNODISCARD
		static ImportFont* Initialize(
            path&& fontPath,
            u8 fontScale);

        KNODISCARD
		u32 GetID() const;

        KNODISCARD
		const path& GetFontPath() const;

        //Returns all gathered font data from the font
        KNODISCARD
		FontData& GetFontData();

        //Returns pixel data receieved from u32 that can be passed to Texture::SetPixelData,
        //set fromAtlas to true if you want this glyph to be received from the created atlas,
        //otherwise it is recreated fresh as a standalone texture
        vector<u8> GetGlyphPixelData(
            u32 codepoint,
            bool fromAtlas = true);

        //Returns pixel data received from a direct glyph that can be passed to Texture::SetPixelData
        //set fromAtlas to true if you want this glyph to be received from the created atlas,
        //otherwise it is recreated fresh as a standalone texture
        vector<u8> GetGlyphPixelData(
            const GlyphData& glyphData,
            bool fromAtlas = true);

        //Draws the selected glyph in 64x128 normalized character range to console,
        //may not represent true width of rasterized glyph
        void DrawGlyphToConsole(u32 codepoint);

        void Destroy();
    private:
        ~ImportFont();

        string Initialize_HarfBuzz(
            vector<u8>&& binaryData,
            FontData& outFontData);

        string GenerateAtlas(FontData& outFontData);

        u32 ID{};

        path fontPath{};
        FontData fontData{};

        hb_draw_funcs drawFuncs;
        hb_blob blob{};
        hb_face face{};
        hb_font font{};
        hb_set unicodes{};
    };
}