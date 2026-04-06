//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "log_utils.hpp"

#include "entity/ee_entity_object.hpp"
#include "graphics/ee_window.hpp"
#include "core/ee_core.hpp"
#include "core/kw_core.hpp"
#include "core/kg_context.hpp"
#include "graphics/models/kg_model.hpp"
#include "graphics/models/kg_model_primitive.hpp"
#include "_internal/opengl/_kg_opengl_model.hpp"
#include "_internal/opengl/_kg_opengl_shader.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Core::WindowContext;
using KalaGraphics::Graphics::Model_Primitive;
using KalaGraphics::Internal::OpenGL::shader_name;
using KalaGraphics::Internal::OpenGL::OpenGL_Shader;
using KalaGraphics::Internal::OpenGL::OpenGL_ShaderType;
using KalaGraphics::Internal::OpenGL::OpenGL_ShaderData;

using std::unique_ptr;
using std::make_unique;
using std::to_string;

namespace ElypsoEngine::Entity
{
    static ElypsoRegistry<Object> registry{};

    ElypsoRegistry<Object>& Object::GetRegistry() { return registry; }

    Object* Object::Initialize(
        string_view modelName,
        const path& shaderFolder,
        u32 windowID,
        const vec3& pos,
        const vec3& rot,
        const vec3& size,
        CubeDetails cDet)
    {
        EngineWindow* ew = EngineWindow::GetRegistry().createdContent[windowID].get();
        if (!ew)
        {
            Log::Print(
                "Failed to create primitive of type 'cube' because the window ID was not found!",
                "EE_OBJECT",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        WindowContext* kgctx = WindowContext::GetRegistry().createdContent[ew->GetContextID()].get();
        if (!kgctx)
        {
            Log::Print(
                "Failed to create primitive of type 'cube' because the window did not contain a valid KalaGraphics context!",
                "EE_OBJECT",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }

        auto& ctx = kgctx->GetWindowContextData();

        //pre-sync to ensure kg gets the highest id
        EngineCore::SyncID();

        uintptr_t context_gl = ctx.context_gl.value_or({});
        VkSurfaceKHR context_vk = ctx.context_vk_surface.value_or({}); 

        u32 shaderID{};
        if (context_gl != 0)
        {
            for (const auto& s : OpenGL_Shader::GetRegistry().runtimeContent)
            {
                if (s->GetName() == shader_name)
                {
                    shaderID = s->GetID();
                    break;
                }
            }
            if (shaderID == 0)
            {
                OpenGL_ShaderData vert =
                {
                    .shaderPath = shaderFolder / (string(shader_name) + ".vert"),
                    .shaderData = {},
                    .type = OpenGL_ShaderType::SHADER_VERTEX
                };
                OpenGL_ShaderData frag =
                {
                    .shaderPath = shaderFolder / (string(shader_name) + ".frag"),
                    .shaderData = {},
                    .type = OpenGL_ShaderType::SHADER_FRAGMENT
                };

                OpenGL_Shader* shader = OpenGL_Shader::Initialize(
                    kgctx->GetID(), 
                    shader_name, 
                    {
                        vert,
                        frag,
                        {}
                    });

                shaderID = shader->GetID();
            }
        }
        else if (context_vk)
        {
            KalaWindowCore::ForceClose(
                "Not implemented",
                "Feature \"Create vulkan object\" is not yet implemented!");

            return nullptr;
        }
        else
        {
            KalaWindowCore::ForceClose(
                "Not implemented",
                "Feature \"Create software rendered object\" is not yet implemented!");

            return nullptr;
        }

        Model_Primitive* modelPtr = Model_Primitive::CreatePrimitive(
            modelName,
            kgctx->GetID(),
            pos,
            rot,
            size,
            cDet);

        if (!modelPtr) return nullptr;

        unique_ptr<Object> newObject = make_unique<Object>();
        Object* objectPtr = newObject.get();

        if (!objectPtr->SetName(modelName)) return nullptr;

        //post-sync to ensure ee gets the highest id from kw
        EngineCore::SyncID();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
        KalaWindowCore::SetGlobalID(newID);

        objectPtr->ID = newID;
        objectPtr->contextID = kgctx->GetID();
        objectPtr->modelID = modelPtr->GetID();

        Object::GetRegistry().AddContent(newID, std::move(newObject));

        Log::Print(
            "Created new object '" + string(modelName) + "' with ID '" + to_string(newID) + "'!",
            "EE_OBJECT",
            LogType::LOG_SUCCESS);

        return objectPtr;
    }

    Object* Object::Initialize(
        string_view modelName,
        const path& shaderFolder,
        u32 windowID,
        const vec3& pos,
        const vec3& rot,
        const vec3& size,
        PyramidDetails pDet)
    {
        KalaWindowCore::ForceClose(
            "Not implemented",
            "Feature \"initialize pyramid object\" is not yet implemented!");

        return nullptr;
    }

    Object* Object::Initialize(
        string_view modelName,
        const path& shaderFolder,
        u32 windowID,
        const vec3& pos,
        const vec3& rot,
        const vec3& size,
        SphereDetails sDet)
    {
        KalaWindowCore::ForceClose(
            "Not implemented",
            "Feature \"initialize sphere object\" is not yet implemented!");

        return nullptr;
    }

    Object* Object::Initialize(
        string_view modelName,
        const path& shaderFolder,
        u32 windowID,
        const vec3& pos,
        const vec3& rot,
        const vec3& size,
        TorusDetails sDet)
    {
        KalaWindowCore::ForceClose(
            "Not implemented",
            "Feature \"initialize torus object\" is not yet implemented!");

        return nullptr;
    }

    Object* Object::Initialize(
        string_view modelName,
        const path& shaderFolder,
        u32 windowID,
        const vec3& pos,
        const vec3& rot,
        const vec3& size,
        const path& modelPath)
    {
        KalaWindowCore::ForceClose(
            "Not implemented",
            "Feature \"initialize standard object\" is not yet implemented!");

        return nullptr;
    }

    bool Object::SetName(string_view newName)
    {
        if (newName.empty())
        {
            Log::Print(
                "Failed to set new model name because it was empty!",
                "EE_OBJECT",
                LogType::LOG_ERROR,
                2);

            return false;
        }
        if (newName.size() > 50)
        {
            Log::Print(
                "Failed to set new model name because it was too long!",
                "EE_OBJECT",
                LogType::LOG_ERROR,
                2);

            return false;
        }

        name = newName;

        return true;
    }
    const string& Object::GetName() const { return name; }

    u32 Object::GetID() const { return ID; }
    u32 Object::GetContextID() const { return contextID; }
    u32 Object::GetModelID() const { return modelID; }

    void Object::Update()
    {

    }

    Object::~Object()
    {
        Log::Print(
            "Destroying model '" + name + "' with ID '" + to_string(ID) + "'.",
            "EE_OBJECT",
            LogType::LOG_INFO);
    }
}