//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "core_utils.hpp"

#include "core/kg_registry.hpp"

namespace KalaGraphics::Graphics
{
    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::default_delete;

    class LIB_API HitTest
    {
    friend class GraphicsContext;
    friend class Viewport;
    friend class Mesh;
    friend struct default_delete<HitTest>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<HitTest>& GetRegistry();

        u32 GetID() const;
        u32 GetContextID() const;
        u32 GetViewportID() const;
        u32 Get3DMeshID() const;
        u32 Get2DMeshID() const;
    private:
        ~HitTest();

        void Destroy();

        static HitTest* Initialize(u32 contextID);

        void Update();

        u32 ID{};
        u32 contextID{};

        u32 viewportID{};
        u32 lastViewportID{};

        u32 mesh3DID{};
        u32 lastMesh3DID{};

        u32 mesh2DID{};
        u32 lastMesh2DID{};
    };
}