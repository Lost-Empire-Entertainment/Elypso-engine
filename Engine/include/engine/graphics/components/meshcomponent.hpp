//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>

//external
#include "glad.h"
#include "gameobject.hpp"
#include "glm.hpp"

//engine
#include "component.hpp"

namespace Graphics::Components
{
    using std::vector;
    using glm::vec3;
    using glm::vec2;

    struct AssimpVertex
    {
        vec3 pos{};
        vec3 normal{};
        vec2 texCoords{};
        vec3 tangent{};
        vec3 bitangent{};
        int boneIDs = 4;
        float weights = 4;
    };

    class AssimpMesh
    {
    public:
        vector<AssimpVertex> vertices;
        vector<unsigned int> indices;

        AssimpMesh(
            vector<AssimpVertex> vertices,
            vector<unsigned int> indices) :
            vertices(vertices),
            indices(indices) {
        }
    };

    class MeshComponent : public Component
    {
    public:
        enum class MeshType
        {
            empty,             //Gameobject which doesnt have any removable components
            model,             //Gameobject whose mesh component has a model
            audio,             //Gameobject with an audio component and its billboard icon
            camera,            //Gameobject with a camera component and its billboard icon
            point_light,       //Gameobject with a light component switched to point light and its billboard icon
            spot_light,        //Gameobject with a light component switched to spotlight and its billboard icon
            directional_light, //Gameobject with a light component switched to directional light and its billboard icon
            billboard,         //Internal billboard gameobject attached to light gameobjects, camera and audio object
            border,            //Internal gameobject used to render the border for selected gameobject
            actionTex,         //Internal gameobject used to render textures for axis and interaction type for selected gameobject
            skybox             //Internal gameobject used for rendering the skybox
        };

        MeshComponent(
            const MeshType& type,
            const GLuint& VAO,
            const GLuint& VBO,
            const GLuint& EBO) : 
            type(type), 
            VAO(VAO), 
            VBO(VBO), 
            EBO(EBO) {}

        ~MeshComponent()
        {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

        void SetMeshPath(const string& newPath) { meshPath = newPath; }
        const string& GetMeshPath() const { return meshPath; }

        void SetMeshType(const MeshType& newType) { type = newType; }
        void SetVAO(const GLuint& newVAO) { VAO = newVAO; }
        void SetVBO(const GLuint& newVBO) { VBO = newVBO; }
        void SetEBO(const GLuint& newEBO) { EBO = newEBO; }
        void SetVertices(const vector<AssimpVertex>& newVertices) { vertices = newVertices; }
        void SetIndices(const vector<unsigned int>& newIndices) { indices = newIndices; }

        const MeshType& GetMeshType() const { return type; }
        const GLuint& GetVAO() const { return VAO; }
        const GLuint& GetVBO() const { return VBO; }
        const GLuint& GetEBO() const { return EBO; }
        const vector<AssimpVertex>& GetVertices() const { return vertices; }
        const vector<unsigned int>& GetIndices() const { return indices; }

        type_index GetType() const override { return typeid(MeshComponent); }
    private:
        MeshType type;
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        vector<AssimpVertex> vertices;
        vector<unsigned int> indices;
        string meshPath;
    };
}