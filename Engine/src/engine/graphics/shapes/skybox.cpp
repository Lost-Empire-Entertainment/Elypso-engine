//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "stb_image.h"

//engine
#include "skybox.hpp"
#include "console.hpp"
#include "render.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using Graphics::Components::Mesh;
using Graphics::Components::Material;
using MeshType = Graphics::Components::Mesh::MeshType;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

using glm::mat3;

#if ENGINE_MODE
using Graphics::GUI::GUISceneWindow;
#endif

namespace Graphics::Shape
{
	shared_ptr<GameObject> Skybox::InitializeSkybox(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& vertShader,
		const string& fragShader,
		string& thisName,
		unsigned int& thisId,
		const bool& isEnabled)
	{
        auto obj = GameObject::Create(
            "Skybox",
            10000003,
            true,
            pos,
            rot,
            scale);

        float vertices[] =
        {        
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        auto mesh = obj->AddComponent<Mesh>(true, Mesh::MeshType::border);
        mesh->Initialize(Mesh::MeshType::border, vertices, sizeof(vertices));

        auto material = obj->AddComponent<Material>();
        material->Initialize(
            (path(Engine::filesPath) / "shaders" / vertShader).string(),
            (path(Engine::filesPath) / "shaders" / fragShader).string());

        Shader skyboxShader = material->GetShader();
        skyboxShader.Use();
        skyboxShader.SetInt("skybox", 0);

        GameObjectManager::SetSkybox(obj);

#if ENGINE_MODE
        GUISceneWindow::UpdateCounts();
#endif

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Successfully initialized " + obj->GetName() + " with ID " + to_string(obj->GetID()) + "\n");

        return obj;
	}

    void Skybox::AssignSkyboxTextures(vector<string> textures, bool flipTextures)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            stbi_set_flip_vertically_on_load(flipTextures);
            unsigned char* data = stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                GLenum format{};
                if (nrChannels == 1) format = GL_RED;
                else if (nrChannels == 3) format = GL_RGB;
                else if (nrChannels == 4) format = GL_RGBA;

                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    format,
                    width,
                    height,
                    0,
                    format,
                    GL_UNSIGNED_BYTE,
                    data);
                stbi_image_free(data);
            }
            else
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Error: Failed to load cubemap at '" + textures[i] + "'.\n");
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        skyboxTexture = textureID;
    }

	void Skybox::RenderSkybox(
        const shared_ptr<GameObject>&obj,
        mat4& view,
        const mat4& projection)
	{
        auto material = obj->GetComponent<Material>();
        auto mesh = obj->GetComponent<Mesh>();
        if (!material
            || !mesh)
        {
            return;
        }

        glDepthFunc(GL_LEQUAL);

        glDepthMask(GL_FALSE);

        Shader skyboxShader = material->GetShader();
        skyboxShader.Use();
        view = mat4(mat3(Render::camera.GetViewMatrix()));
        skyboxShader.SetMat4("view", view);
        skyboxShader.SetMat4("projection", projection);

        glBindVertexArray(mesh->GetVAO());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);

        glDepthFunc(GL_LESS);
	}
}