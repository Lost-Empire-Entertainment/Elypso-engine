//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <map>
#include <fstream>
#include <chrono>
#include <iomanip>

//external
#include "glad.h"
#include "quaternion.hpp"
#include "matrix_transform.hpp"
#include "matrix_decompose.hpp"
#include "postprocess.h"
#include "stb_image.h"
#include "Assimp/Importer.hpp"

//engine
#include "importer.hpp"
#include "render.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "core.hpp"
#include "console.hpp"
#include "selectobject.hpp"
#include "fileUtils.hpp"
#include "transformcomponent.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lightcomponent.hpp"

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
using std::filesystem::directory_iterator;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

using Graphics::Render;
using Graphics::Shader;
using Graphics::Texture;
using Graphics::Components::TransformComponent;
using Graphics::Components::MeshComponent;
using Graphics::Components::MaterialComponent;
using Graphics::Components::LightComponent;
using Graphics::Components::AssimpMesh;
using Graphics::Components::AssimpVertex;
using MeshType = Graphics::Components::MeshComponent::MeshType;
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
        const string& diffTexture,
        const string& specTexture,
        const string& normalTexture,
        const string& heightTexture,
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
        
        if (!ValidateScene(scene)) return;

        aiNode* topLevelNode = scene->mRootNode->mChildren[0];

        ProcessNode(
            name,
            id,
            isEnabled,
            pos,
            rot,
            scale,
            modelPath,
            diffTexture,
            specTexture,
            normalTexture,
            heightTexture,
            topLevelNode,
            scene);
    }

    void Importer::ProcessNode(
        string& name,
        unsigned int& id,
        const bool& isEnabled,
        const vec3& pos,
        const vec3& rot,
        const vec3& scale,
        const string& modelPath,
        const string& diffTexture,
        const string& specTexture,
        const string& normalTexture,
        const string& heightTexture,
        aiNode* node,
        const aiScene* scene)
    {
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

        aiMesh* mesh = scene->mMeshes[0];
        AssimpMesh newMesh = ProcessMesh(mesh, scene);

        if (id == tempID) id = GameObject::nextID++;

        string nameAndExtension = name + ".txt";

        string scenePath = path(Engine::scenePath).parent_path().filename().string();
        string parentFolderName = path(modelPath).parent_path().filename().string();
        string finalTxtPath = (path("scenes") / scenePath / "gameobjects" / parentFolderName / nameAndExtension).string();

        shared_ptr<GameObject> newChild = Model::Initialize(
            nodePosition,
            nodeRotation,
            nodeScale,
            finalTxtPath,
            modelPath,
            diffTexture,
            specTexture,
            normalTexture,
            heightTexture,
            newMesh.vertices,
            newMesh.indices,
            name,
            id,
            isEnabled);
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

    bool Importer::ValidateScene(const aiScene* scene)
    {
        if (MeshCount(scene) == 1)
        {
            aiMesh* mesh = scene->mMeshes[0];
            if (AnimMeshCount(mesh) > 0)
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Assimp error: Cannot import models with animated meshes! This feature is not yet supported.\n");
                return false;
            }
            if (BoneCount(mesh) > 0)
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Assimp error: Cannot import models with bones! This feature is not yet supported.\n");
                return false;
            }
            if (VerticeCount(mesh) == 0)
            {
                ConsoleManager::WriteConsoleMessage(
                    Caller::FILE,
                    Type::EXCEPTION,
                    "Assimp error: Cannot import models with no vertices!\n");
                return false;
            }
        }

        else if (MeshCount(scene) == 0)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: Cannot import empty model file!\n");
            return false;
        }
        else if (MeshCount(scene) > 1)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: Cannot import models with more than one mesh! This feature is not yet supported.\n");
            return false;
        }

        if (MaterialCount(scene) == 0)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: Cannot import models with no materials! This feature is not yet supported.\n");
            return false;
        }
        else if (MaterialCount(scene) > 1)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: Cannot import models with more than one material! This feature is not yet supported.\n");
            return false;
        }

        if (AnimationCount(scene) > 0)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: Cannot import models with animations! This feature is not yet supported.\n");
            return false;
        }
        if (CameraCount(scene) > 0)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: Cannot import models with cameras! This feature is not yet supported.\n");
            return false;
        }
        if (LightCount(scene) > 0)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: Cannot import models with lights! This feature is not yet supported.\n");
            return false;
        }
        if (SkeletonCount(scene) > 0)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: Cannot import models with skeletons! This feature is not yet supported.\n");
            return false;
        }
        if (TextureCount(scene) > 0)
        {
            ConsoleManager::WriteConsoleMessage(
                Caller::FILE,
                Type::EXCEPTION,
                "Assimp error: Cannot import models with textures! This feature is not yet supported. Please import textures separately.\n");
            return false;
        }

        return true;
    }

    //
    // CHECK MESH DATA
    //

    int Importer::AnimMeshCount(const aiMesh* mesh)
    {
        return mesh->mNumAnimMeshes;
    }
    int Importer::BoneCount(const aiMesh* mesh)
    {
        return mesh->mNumBones;
    }
    int Importer::VerticeCount(const aiMesh* mesh)
    {
        return mesh->mNumVertices;
    }

    //
    // CHECK SCENE DATA
    //

    int Importer::AnimationCount(const aiScene* scene)
    {
        return scene->mNumAnimations;
    }
    int Importer::CameraCount(const aiScene* scene)
    {
        return scene->mNumCameras;
    }
    int Importer::LightCount(const aiScene* scene)
    {
        return scene->mNumLights;
    }
    int Importer::MaterialCount(const aiScene* scene)
    {
        return scene->mNumMaterials;
    }
    int Importer::MeshCount(const aiScene* scene)
    {
        return scene->mNumMeshes;
    }
    int Importer::SkeletonCount(const aiScene* scene)
    {
        return 0;
    }
    int Importer::TextureCount(const aiScene* scene)
    {
        return scene->mNumTextures;
    }
}
