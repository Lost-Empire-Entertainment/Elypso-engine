//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "Stb_image/stb_image.h"

//engine
#include "skybox.hpp"
#include "console.hpp"
#include "render.hpp"
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using Graphics::Shape::Mesh;
using MeshType = Graphics::Shape::Mesh::MeshType;
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
		shared_ptr<Transform> transform = make_shared<Transform>(pos, rot, scale);

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

        shared_ptr<Mesh> mesh = make_shared<Mesh>(true, MeshType::skybox, VAO, VBO, 0);

        Shader skyboxShader = Shader::LoadShader(vertShader, fragShader);

        skyboxShader.Use();
        skyboxShader.SetInt("skybox", 0);

        shared_ptr<Material> mat = make_shared<Material>();
        mat->AddShader(vertShader, fragShader, skyboxShader);

        shared_ptr<BasicShape_Variables> skybox =
            make_shared<BasicShape_Variables>(32.0f);

        shared_ptr<GameObject> obj = make_shared<GameObject>(
            true,
            name,
            id,
            isEnabled,
            transform,
            mesh,
            mat,
            skybox);

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
        glDepthFunc(GL_LEQUAL);

        glDepthMask(GL_FALSE);

        Shader skyboxShader = obj->GetMaterial()->GetShader();
        skyboxShader.Use();
        view = mat4(mat3(Render::camera.GetViewMatrix()));
        skyboxShader.SetMat4("view", view);
        skyboxShader.SetMat4("projection", projection);

        glBindVertexArray(obj->GetMesh()->GetVAO());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);

        glDepthFunc(GL_LESS);
	}
}