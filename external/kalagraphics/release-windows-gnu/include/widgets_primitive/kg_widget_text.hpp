//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <cfloat>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/kg_registry.hpp"

struct VkCommandBuffer_T;
using VkCommandBuffer = VkCommandBuffer_T*;

namespace KalaGraphics::Graphics
{
    class Viewport;
}

namespace KalaGraphics::CompositeWidgets
{
    class Button;
}

namespace KalaGraphics::PrimitiveWidgets
{
    using KalaHeaders::KalaMath::vec2;

    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::string;
    using std::string_view;
    using std::vector;
    using std::pair;
    using std::default_delete;

    static constexpr u16 MAX_CHARACTERS = 1024;

    static constexpr u16 MAX_LINES = 1024;
    static constexpr u16 MAX_LINE_WIDTH = 8192;
    static constexpr u16 MIN_LINE_WIDTH = 32;
    static constexpr u16 MAX_LINE_HEIGHT = 255;
    static constexpr u16 MIN_LINE_HEIGHT = 1;

    static constexpr f32 MAX_TEXT_MULTIPLIER_SIZE = 10.0f;
    static constexpr f32 MIN_TEXT_MULTIPLIER_SIZE = 0.1f;

    static constexpr f64 CURSOR_BLINK_INTERVAL_S = 0.5;

    static constexpr f32 DRAG_THRESHOLD_PX = 5;

    enum class TextClipType : u8
    {
        //when a glyph/text exceeds line width,
        //allow it to continue beyond the line width and max lines
        C_OVERFLOW = 0,
        //if word width + existing line content exceeds line width
        //but words own width is less than line width then word gets pushed to next line,
        //otherwise the word is not drawn at all, word is not drawn if it exceeds max lines
        C_CLIPPED = 1
    };

    enum class TextFieldType : u8
    {
        //supports all characters, including emojis
        F_ANY                   = 0,
        //only supports text and numbers
        F_TEXT_ONLY             = 1,
        //only supports integers, floats and doubles
        F_NUMBER_ONLY           = 2,
        //only supports integers
        F_INTEGER_ONLY          = 3,
        //only supports floats
        F_FLOAT_ONLY            = 4,
        //only supports floats and doubles
        F_FLOAT_AND_DOUBLE_ONLY = 5,
        //displays written value as stars, supports all characters
        F_PASSWORD              = 6
    };

    enum class TextAlignmentType : u8
    {
        A_TOP_LEFT    = 0,
        A_CENTER_LEFT = 1,
        A_BOTTOM_LEFT = 2,

        A_TOP_CENTER    = 3,
        A_CENTER        = 4,
        A_BOTTOM_CENTER = 5,

        A_TOP_RIGHT    = 6,
        A_CENTER_RIGHT = 7,
        A_BOTTOM_RIGHT = 8
    };

    struct GlyphRasterData
    {
        u32 utf;

        vec2 penPos{};
        vec2 glyphPos{};
        vec2 glyphSize{};
    };

    struct CursorData
    {
        //the start of a glyph, if set to size of glyphRasterData
        //then this means end of last glyph
        i32 characterSlot = -1;

        //which line are we currently on, 1 is top line, 0 is invalid
        u32 line = 1;

        //do we need to move cursor position
        bool isCursorPosDirty = true;
        //center of cursor
        vec2 pos{};

        //are we currently drawing the cursor
        bool isCursorOn{};
        //when was cursor last turned on/off
        f64 timeSinceLastStateSwitch{};

        //where cursor back pixels were last captured
        vec2 cursorBackPos{};
        //store old pixels behind cursor so when cursor goes off we can still use them
        vector<u8> cursorBackPixels{};
    };

    struct HighlightData
    {
        //what character slots are currently highlighted
        pair<i32, i32> highlightRange = { -1, -1 };

        //do we need to redraw highlight
        bool isHighlightDirty{};

        //where highlight drag detection starts from
        vec2 dragStartMousePos{};

        //where highlight back pixels were last captured
        vec2 highlightBackPos{};
        //how big was the last highlight range
        vec2 highlightBackSize{};
        //store old pixels behind highlight so when highlight ends we can still use them
        vector<u8> highlightBackPixels{};
    };

    class LIB_API Text
    {
    friend class KalaGraphics::Graphics::Viewport;
    friend class KalaGraphics::CompositeWidgets::Button;
    friend struct default_delete<Text>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<Text>& GetRegistry();

        KNODISCARD
		static bool IsVerboseLoggingEnabled();
        static void SetVerboseLoggingState(bool state);

        KNODISCARD
        static Text* Initialize(
            u32 fontID,
            u32 viewportID);

        KNODISCARD
        u32 GetID() const;

        KNODISCARD
        u32 GetFontID() const;
        void SetFontID(u32 newValue);

        KNODISCARD
        u32 GetShaderID() const;
        KNODISCARD
        u32 GetTextureID() const;
        KNODISCARD
        u32 GetMeshID() const;

        //Returns true if this text widget can be selected and edited via cursor position
        KNODISCARD
        bool CanEdit() const;
        void SetEditState(bool newValue);

        KNODISCARD
        TextClipType GetClipType() const;
        void SetClipType(TextClipType newValue);

        KNODISCARD
        TextFieldType GetFieldType() const;
        void SetFieldType(TextFieldType newValue);

        KNODISCARD
        TextAlignmentType GetAlignmentType() const;
        void SetAlignmentType(TextAlignmentType newValue);

        //Which character is the cursor relative to right now?
        //If pos is -1 then cursor is disabled,
        //otherwise pos represents the start of a glyph, 
        //if cursor pos is size of characters then cursor pos is at the end of the last glyph
        KNODISCARD
        i32 GetCursorPos() const;
        //Move the cursor to a selected utf in this text widget,
        //if another text widget already has a cursor then it is removed and added here,
        //finds first utf with given value, if first is already selected then it jumps to next one and so on,
        //change second higher than -1 to lock to the Nth character of the utf you chose 
        void SetCursorPosByUTF(
            i32 targetUTF,
            i32 targetUTFSlot = -1);
        //Manually set cursor pos to a known character slot,
        //if another text widget already has a cursor then it is removed and added here,
        void SetCursorPosBySlot(i32 targetSlot);

        //Get start and end character to determine which glyphs are currently highlighted
        KNODISCARD
        pair<i32, i32> GetHighlightRange() const;
        //Choose which characters to manually highlight
        void SetHighlightRange(pair<i32, i32> newValue);

        KNODISCARD
        f32 GetTextSizeMultiplier() const;
        //Set new text size multiplier, clamped from 0.1 to 10.0
        void SetTextSizeMultiplier(f32 newValue);

        KNODISCARD
        u16 GetLineWidth() const;
        //Set new line width, ignored if clip type is overflow
        void SetLineWidth(u16 newValue);

        KNODISCARD
        u16 GetLineHeight() const;

        KNODISCARD
        u16 GetMaxLines() const;
        //Set new max line count, ignored if clip type is overflow
        void SetMaxLines(u16 newValue);

        //Get max allowed characters of this text
        KNODISCARD
        u16 GetMaxCharacters() const;
        //Set max allowed characters of this text, clamped from 1 to MAX_CHARACTERS
        void SetMaxCharacters(u16 newValue);

        //Returns true if this text widget can only contain numbers
        KNODISCARD
        bool IsNumberField() const;

        //Get the smallest allowed value of this numerical field,
        //only applies to integers, floats and doubles,
        //cannot be set bigger than max
        KNODISCARD
        f64 GetNumberMin() const;
        void SetNumberMin(f64 newValue);

        //Get the highest allowed value of this numerical field,
        //only applies to integers, floats and doubles
        //cannot be set lower than min
        KNODISCARD
        f64 GetNumberMax() const;
        void SetNumberMax(f64 newValue);

        //Get string converted from UTF characters,
        //set getDisplayed to true to get displayed text,
        //otherwise it returns internal real text
        KNODISCARD
        string GetText(bool getDisplayed = true) const;
        //Directly append or prepend string to this text widget,
        //change startChar to decide which character to add from.
        //Set addDisplayed to true to add as displayed text,
        //otherwise it adds to internal real text
        void AddText(
            string_view newValue,
            u32 startChar = 0,
            bool back = true,
            bool addDisplayed = true);
        //Remove amount of characters from front or back,
        //change startChar to decide which character to remove from.
        //Set removeDisplayed to true to remove displayed text,
        //otherwise it removes from internal real text
        void RemoveText(
            u32 count,
            u32 startChar = 0,
            bool back = true,
            bool removeDisplayed = true);
        //Overwrite existing string with new value.
        //Set setDisplayed to true to set as displayed text,
        //otherwise it sets as internal real text
        void SetText(
            string_view newValue,
            bool setDisplayed = true);

        //Returns UTF characters instead of string,
        //set getDisplayed to true to get displayed text,
        //otherwise it returns internal real text
        KNODISCARD
        vector<u32> GetUTF(bool getDisplayed = true) const;
        //Directly append or prepend UTF to this text widget,
        //change startChar to decide which character to add from.
        //Set addDisplayed to true to add as displayed text,
        //otherwise it adds to internal real text
        void AddUTF(
            vector<u32>&& newValue,
            u32 startChar = 0,
            bool back = true,
            bool addDisplayed = true);
        //Overwrite existing UTF with new value.
        //Set setDisplayed to true to set as displayed text,
        //otherwise it sets as internal real text
        void SetUTF(
            vector<u32>&& newValue,
            bool setDisplayed = true);
    
        void Destroy();
    private:
        ~Text();

        void Update(VkCommandBuffer buffer);
        void UpdateCursor(f64 deltaTime);

        u32 ID{};
        u32 fontID{};
        u32 shaderID{};
        u32 textureID{};
        u32 meshID{};
        
        u32 buttonWidgetID{};

        bool canEdit{};
        bool isTextDirty = true;

        TextClipType clipType{};
        TextFieldType fieldType{};
        TextAlignmentType alignmentType = TextAlignmentType::A_CENTER;

        CursorData cursorData{}; //where to render the cursor
        HighlightData highlightData{}; //which characters are selected

        f32 textSizeMultiplier = 1.0f;

        u16 lineWidth = 128;
        u16 lineHeight{};
        u16 cursorOffset = 16;
        u16 maxLines = 1;

        u16 maxCharacters = 32;

        f64 numberMin = -DBL_MAX;
        f64 numberMax = DBL_MAX;

        //start and end character of each line
        vector<pair<u32, u32>> lineData{};

        vector<GlyphRasterData> displayedText{};
        vector<u32> realText{};
    };
}