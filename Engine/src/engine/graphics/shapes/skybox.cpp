//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

//external
#include "stb_image.h"

//engine
#include "skybox.hpp"
#include "console.hpp"
#include "render.hpp"
#include "transformcomponent.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lightcomponent.hpp"
#include "sceneFile.hpp"
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using glm::mat3;
using std::filesystem::exists;

using Graphics::Components::TransformComponent;
using Graphics::Components::MeshComponent;
using Graphics::Components::MaterialComponent;
using Graphics::Components::LightComponent;
using MeshType = Graphics::Components::MeshComponent::MeshType;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using EngineFile::SceneFile;
#if ENGINE_MODE
using Graphics::GUI::GUISceneWindow;
#endif

namespace Graphics::Shape
{
	shared_ptr<GameObject> Skybox::InitializeSkybox()
	{
        auto obj = make_shared<GameObject>("Skybox", 10000003);
        auto transform = obj->AddComponent<TransformComponent>();
        transform->SetOwner(obj);
        transform->SetPosition(vec3(0));
        transform->SetRotation(vec3(0));
        transform->SetScale(vec3(1));
        obj->SetEnableState(true);

        float skyboxVertices[] =
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

        unsigned int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(skyboxVertices),
            &skyboxVertices,
            GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            3 * sizeof(float),
            (void*)0);

        auto mesh = obj->AddComponent<MeshComponent>(
            MeshType::skybox, 
            VAO, 
            VBO, 
            0);
        mesh->SetOwner(obj);

        string vert = (path(Engine::filesPath) / "shaders" / "Skybox.vert").string();
        string frag = (path(Engine::filesPath) / "shaders" / "Skybox.frag").string();

        if (!exists(vert)
            || !exists(frag))
        {
            Engine::CreateErrorPopup("One of the shader paths for skybox is invalid!");
        }

        Shader skyboxShader = Shader::LoadShader(vert, frag);

        skyboxShader.Use();
        skyboxShader.SetInt("skybox", 0);

        auto mat = obj->AddComponent<MaterialComponent>();
        mat->SetOwner(obj);
        mat->AddShader(vert, frag, skyboxShader);

        GameObjectManager::SetSkybox(obj);

#if ENGINE_MODE
        GUISceneWindow::UpdateCounts();
#endif

        ConsoleManager::WriteConsoleMessage(
            Caller::FILE,
            Type::DEBUG,
            "Successfully initialized skybox gameobject!\n");

        return obj;
	}

    void Skybox::AssignSkyboxTextures(vector<string> textures, bool flipTextures)
    {
        int width, height, nrChannels;
        bool invalidTexture = false;

        vector<string> resolution{};

        int firstChannelCount{};
        vector<int> channelCount{};

        //first for loop loops through all texture files and stores their width and height
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            string texturePath = path(textures[i]).stem().string() != "skybox_default"
                ? (path(Engine::projectPath) / textures[i]).string()
                : (path(Engine::filesPath) / textures[i]).string();
            unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                string value = to_string(width) + "x" + to_string(height);
                resolution.push_back(value);
                channelCount.push_back(nrChannels);

                if (i == 0) firstChannelCount = nrChannels;
            }
            else
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Error: Failed to load skybox texture at '" + texturePath + "'! All skybox textures have been reset.\n");

                invalidTexture = true;
                stbi_image_free(data);
            }
        }

        //second for loop checks if all the resolution values are the same but only if all textures loaded correctly
        if (!invalidTexture)
        {
            string firstResolution{};
            string firstExtension{};
            for (unsigned int i = 0; i < resolution.size(); i++)
            {
                string texture = textures[i];
                string value = resolution[i];

                if (i == 0)
                {
                    firstResolution = resolution[i];
                    firstExtension = path(textures[i]).extension().string();
                    continue;
                }

                if (resolution[i] != firstResolution)
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::EXCEPTION,
                        "Error: One or more skybox textures don't have the same resolution as the rest! All skybox textures have been reset.\n");

                    invalidTexture = true;
                    break;
                }

                if (path(textures[i]).extension().string() != firstExtension)
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::EXCEPTION,
                        "Error: One or more skybox textures don't have the same extension as the rest! All skybox textures have been reset.\n");

                    invalidTexture = true;
                    break;
                }

                if (channelCount[i] != firstChannelCount)
                {
                    ConsoleManager::WriteConsoleMessage(
                        Caller::FILE,
                        Type::EXCEPTION,
                        "Error: One or more skybox textures don't have the same channel count as the rest! All skybox textures have been reset.\n");

                    invalidTexture = true;
                    break;
                }
            }
        }

        //third for loop is ran if one or more textures are incorrect, this resets all skybox textures
        vector<string> finalTexturesList{};
        bool changedTextureList = false;
        if (invalidTexture)
        {
            finalTexturesList.push_back((path("textures") / "skybox_default.png").string());
            finalTexturesList.push_back((path("textures") / "skybox_default.png").string());
            finalTexturesList.push_back((path("textures") / "skybox_default.png").string());
            finalTexturesList.push_back((path("textures") / "skybox_default.png").string());
            finalTexturesList.push_back((path("textures") / "skybox_default.png").string());
            finalTexturesList.push_back((path("textures") / "skybox_default.png").string());

            SceneFile::skyboxTexturesMap.clear();
            SceneFile::skyboxTexturesMap["right"] = (path("textures") / "skybox_default.png").string();
            SceneFile::skyboxTexturesMap["left"] = (path("textures") / "skybox_default.png").string();
            SceneFile::skyboxTexturesMap["top"] = (path("textures") / "skybox_default.png").string();
            SceneFile::skyboxTexturesMap["bottom"] = (path("textures") / "skybox_default.png").string();
            SceneFile::skyboxTexturesMap["front"] = (path("textures") / "skybox_default.png").string();
            SceneFile::skyboxTexturesMap["back"] = (path("textures") / "skybox_default.png").string();

            changedTextureList = true;
        }
        else 
        {
            finalTexturesList = textures;

            ConsoleManager::WriteConsoleMessage(
                Caller::INPUT,
                Type::INFO,
                "Successfully applied new skybox textures!\n");
        }

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        //fourth and final for loop finally loads each texture
        for (unsigned int i = 0; i < finalTexturesList.size(); i++)
        {
            stbi_set_flip_vertically_on_load(flipTextures);

            string texturePath = path(finalTexturesList[i]).stem().string() != "skybox_default"
                ? (path(Engine::projectPath) / finalTexturesList[i]).string()
                : (path(Engine::filesPath) / finalTexturesList[i]).string();
            unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

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
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        skyboxTexture = textureID;

        if (changedTextureList) SceneFile::SaveScene();
    }

	void Skybox::RenderSkybox(
        const shared_ptr<GameObject>&obj,
        mat4& view,
        const mat4& projection)
	{
        if (obj == nullptr) Engine::CreateErrorPopup("Skybox gameobject is invalid.");

        glDepthFunc(GL_LEQUAL);

        glDepthMask(GL_FALSE);

        auto mat = obj->GetComponent<MaterialComponent>();
        Shader skyboxShader = mat->GetShader();
        skyboxShader.Use();
        view = mat4(mat3(Render::camera.GetViewMatrix()));
        skyboxShader.SetMat4("view", view);
        skyboxShader.SetMat4("projection", projection);

        auto mesh = obj->GetComponent<MeshComponent>();
        glBindVertexArray(mesh->GetVAO());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);

        glDepthFunc(GL_LESS);
	}
}
