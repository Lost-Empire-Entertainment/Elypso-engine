//Copyright(C) 2024 Lost Empire Entertainment
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
            border,
            actionTex,
            model,
            point_light,
            spot_light,
            directional_light,
            billboard,
            skybox
        };

        MeshComponent(
            const bool& isEnabled,
            const MeshType& type,
            const GLuint& VAO,
            const GLuint& VBO,
            const GLuint& EBO) : 
            isEnabled(isEnabled), 
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

        void AddAssimpMesh(const AssimpMesh& newMesh) { assimpMeshes.push_back(newMesh); }
        const vector<AssimpMesh>& GetAssimpMeshes() const { return assimpMeshes; }

        void SetEnableState(const bool& newIsEnabled) { isEnabled = newIsEnabled; }
        void SetMeshType(const MeshType& newType) { type = newType; }
        void SetVAO(const GLuint& newVAO) { VAO = newVAO; }
        void SetVBO(const GLuint& newVBO) { VBO = newVBO; }
        void SetEBO(const GLuint& newEBO) { EBO = newEBO; }
        void SetVertices(const vector<AssimpVertex>& newVertices) { vertices = newVertices; }
        void SetIndices(const vector<unsigned int>& newIndices) { indices = newIndices; }

        const bool& IsEnabled() const { return isEnabled; }
        const MeshType& GetMeshType() const { return type; }
        const GLuint& GetVAO() const { return VAO; }
        const GLuint& GetVBO() const { return VBO; }
        const GLuint& GetEBO() const { return EBO; }
        const vector<AssimpVertex>& GetVertices() const { return vertices; }
        const vector<unsigned int>& GetIndices() const { return indices; }

        type_index GetType() const override { return typeid(MeshComponent); }

        void Update(float deltaTime) override {}

    private:
        bool isEnabled;
        MeshType type;
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        vector<AssimpVertex> vertices;
        vector<unsigned int> indices;

        vector<AssimpMesh> assimpMeshes;
    };
}