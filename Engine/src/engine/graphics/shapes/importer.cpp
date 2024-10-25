//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <map>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>

//external
#include "glad.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "assimp/postprocess.h"
#include "stb_image.h"

//engine
#include "render.hpp"
#include "importer.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"
#include "console.hpp"
#include "selectobject.hpp"
#include "fileUtils.hpp"

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
using std::filesystem::exists;
using std::filesystem::file_time_type;
using std::filesystem::last_write_time;
using glm::decompose;
using std::ofstream;
using std::put_time;
using std::localtime;
using std::ostringstream;
using std::clock;
using std::chrono::system_clock;

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
using Core::Select;
using Utils::File;

namespace Graphics::Shape
{
    void Importer::Initialize(
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

        unsigned int nodeIndex = 0;

        for (unsigned int i = 0; i < scene->mRootNode->mNumChildren; i++)
        {
            aiNode* topLevelNode = scene->mRootNode->mChildren[i];

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
                topLevelNode,
                scene,
                nodeIndex++);
        }
    }

    void Importer::ProcessNode(
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
        const aiScene* scene,
        unsigned int nodeIndex)
    {
        string nodeName = node->mName.C_Str();
        if (nodeName.empty()) nodeName = "Model";

        name = nodeName;

        //get parent node
        string parentName =
            (node->mParent
            && string(node->mParent->mName.C_Str()) != "RootNode")
            ? node->mParent->mName.C_Str()
            : "";

        //get all child nodes
        string childNames;
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            childNames += node->mChildren[i]->mName.C_Str();
            if (i != node->mNumChildren - 1)
            {
                childNames += ", ";
            }
        }

        //apply the parents transformation to the current node
        aiMatrix4x4 globalTransformation = node->mTransformation;

        if (node->mParent) 
        {
            globalTransformation = node->mParent->mTransformation * node->mTransformation;
        }

        vec3 nodePosition, nodeRotation, nodeScale;
        DecomposeTransform(globalTransformation, nodePosition, nodeRotation, nodeScale);

        //process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            //the node object only contains indices to index the actual objects in the scene. 
            //the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            AssimpMesh newMesh = ProcessMesh(mesh, scene);

            if (id == tempID) id = GameObject::nextID++;

            shared_ptr<GameObject> newChild = Model::Initialize(
                nodePosition,
                nodeRotation,
                nodeScale,
                modelPath,
                vertShader,
                fragShader,
                diffTexture,
                specTexture,
                normalTexture,
                heightTexture,
                newMesh.vertices,
                newMesh.indices,
                shininess,
                nodeName,
                id,
                isEnabled,
                true);
        }

        //after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            string childName = node->mChildren[i]->mName.C_Str();
            if (childName.empty()) childName = "Model";

            ProcessNode(
                childName,
                id,
                isEnabled,
                nodePosition,
                nodeRotation,
                nodeScale,
                modelPath,
                vertShader,
                fragShader,
                diffTexture,
                specTexture,
                normalTexture,
                heightTexture,
                shininess,
                node->mChildren[i],
                scene,
                nodeIndex++);
        }
    }

    AssimpMesh Importer::ProcessMesh(
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

    void Importer::DecomposeTransform(const aiMatrix4x4& transform, vec3& outPosition, vec3& outRotation, vec3& outScale)
    {
        mat4 glmTransform = mat4(
            transform.a1, transform.b1, transform.c1, transform.d1,
            transform.a2, transform.b2, transform.c2, transform.d2,
            transform.a3, transform.b3, transform.c3, transform.d3,
            transform.a4, transform.b4, transform.c4, transform.d4
        );

        vec3 scale, position, skew;
        quat rotation;
        vec4 perspective;

        decompose(glmTransform, scale, rotation, position, skew, perspective);

        vec3 theEulerAngles = eulerAngles(rotation);
        outPosition = position;
        outPosition *= 0.005f;
        outRotation = degrees(theEulerAngles);
        outScale = scale;
    }
}