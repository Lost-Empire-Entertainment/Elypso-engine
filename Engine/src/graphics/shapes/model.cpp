//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <map>
#include <filesystem>

//external
#include "glad.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"
#include "assimp/postprocess.h"
#include "stb_image.h"

//engine
#include "render.hpp"
#include "model.hpp"
#include "modelchild.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"
#include "console.hpp"
#include "selectobject.hpp"

using std::cout;
using std::endl;
using std::to_string;
using std::map;
using glm::translate;
using glm::rotate;
using glm::radians;
using glm::quat;
using glm::scale;
using glm::rotate;
using std::filesystem::path;

using Graphics::Render;
using Graphics::Shader;
using Graphics::Texture;
using Graphics::Shape::Mesh;
using MeshType = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::Material;
using Core::Engine;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Physics::Select;

namespace Graphics::Shape
{
    void Model::Initialize(
        const vec3& pos,
        const vec3& rot,
        const vec3& scale,
        const string& modelPath,
        const string& vertShader,
        const string& fragShader,
        const string& diffTexture,
        const string& specTexture,
        const string& normalTexture,
        const string& heightTexture,
        const float& shininess,
        string& name,
        unsigned int& id,
        const bool& isEnabled)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            modelPath,
            aiProcess_Triangulate
            | aiProcess_GenSmoothNormals
            | aiProcess_FlipUVs
            | aiProcess_CalcTangentSpace);
        //check for errors
        if (!scene
            || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
            || !scene->mRootNode)
        {
            string errorString = importer.GetErrorString();
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: " + errorString + "\n");
            return;
        }

        //process assimps root node recursively
        ProcessNode(
            name, 
            id,
            isEnabled,
            pos, 
            rot, 
            scale,
            modelPath,
            vertShader,
            fragShader,
            diffTexture,
            specTexture,
            normalTexture,
            heightTexture,
            shininess,
            scene->mRootNode, 
            scene);
    }

    void Model::ProcessNode(
        string& name,
        unsigned int& id,
        const bool& isEnabled,
        const vec3& pos,
        const vec3& rot,
        const vec3& scale,
        const string& modelPath,
        const string& vertShader,
        const string& fragShader,
        const string& diffTexture,
        const string& specTexture,
        const string& normalTexture,
        const string& heightTexture,
        const float& shininess,
        aiNode* node,
        const aiScene* scene)
    {
        //process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            //the node object only contains indices to index the actual objects in the scene. 
            //the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            AssimpMesh newMesh = ProcessMesh(mesh, scene);

            if (name == tempName) name = "Model";
            if (id == tempID) id = GameObject::nextID++;

            string modelPathNonRef = modelPath;
            if (modelPathNonRef == "") modelPathNonRef = targetModel;

            shared_ptr<GameObject> newChild = ModelChild::Initialize(
                pos,
                rot,
                scale,
                vertShader,
                fragShader,
                diffTexture,
                specTexture,
                normalTexture,
                heightTexture,
                newMesh.vertices,
                newMesh.indices,
                shininess,
                name,
                id,
                isEnabled);

            newChild->SetDirectory(modelPathNonRef);

            Select::selectedObj = newChild;
            Select::isObjectSelected = true;
        }

        //after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(
                name, 
                id, 
                isEnabled,
                pos,
                rot,
                scale,
                modelPath,
                vertShader,
                fragShader,
                diffTexture,
                specTexture,
                normalTexture,
                heightTexture,
                shininess,
                node->mChildren[i], 
                scene);
        }
    }

    AssimpMesh Model::ProcessMesh(
        aiMesh* mesh,
        const aiScene* scene)
    {
        vector<AssimpVertex> vertices;
        vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            AssimpVertex vertex;
            vec3 vector{};

            //positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.pos = vector * 0.005f;

            //normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }

            //texture coordinates
            if (mesh->mTextureCoords[0])
            {
                vec2 vec{};
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;

                //tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.tangent = vector;

                //bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.bitangent = vector;
            }
            else vertex.texCoords = vec2(0);

            vertices.push_back(vertex);
        }

        //mesh faces
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            //retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        //return a mesh object created from the extracted mesh data
        return AssimpMesh(vertices, indices);
    }
}