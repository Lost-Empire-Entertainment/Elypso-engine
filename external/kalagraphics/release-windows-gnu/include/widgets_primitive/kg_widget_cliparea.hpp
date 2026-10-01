//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>

#include "core_utils.hpp"

#include "core/kg_registry.hpp"

namespace KalaGraphics::PrimitiveWidgets
{
    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::vector;
    using std::string;
    using std::default_delete;

    enum class ClipType : u8
    {
        C_FREE   = 0, //no restrictions, draw outside/inside the clip area
        C_BOUNDS = 1  //cannot draw past/into the clip area
    };

    class LIB_API ClipArea
    {
    friend struct default_delete<ClipArea>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<ClipArea>& GetRegistry();

        KNODISCARD
        static ClipArea* Initialize();

        KNODISCARD
        u32 GetID() const;
        KNODISCARD
        u32 GetShaderID() const;
        KNODISCARD
        u32 GetMeshID() const;

        KNODISCARD
        ClipType GetClipType() const;
        void SetClipType(ClipType newValue);

        KNODISCARD
        bool IsReversed() const;
        //If true, then this clip area doesn't allow to draw inside its area,
        //otherwise it doesn't allow to draw outside its area
        void SetReverseState(bool newValue);
    
        void Destroy();
    private:
        ~ClipArea();

        u32 ID{};
        u32 shaderID{};
        u32 textureID{};
        u32 meshID{};

        ClipType clipType{};

        bool reversed{};
    };
}