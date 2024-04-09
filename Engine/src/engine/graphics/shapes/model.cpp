//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

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

namespace Graphics::Shape
{
    shared_ptr<GameObject> Model::Initialize(
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
        unsigned int& id)
    {
        shared_ptr<Transform> transform = make_shared<Transform>(pos, rot, scale);

        GLuint vao, vbo, ebo;

        shared_ptr<Mesh> mesh = make_shared<Mesh>(MeshType::model, vao, vbo, ebo);

        Shader modelShader = Shader::LoadShader(vertShader, fragShader);

        shared_ptr<Material> mat = make_shared<Material>();
        mat->AddShader(vertShader, fragShader, modelShader);

        vector<AssimpMesh> assimpMeshes;

        path modelPathToString = path(modelPath);
        string modelName = modelPathToString.stem().string();

        if (name == tempName) name = modelName;
        if (id == tempID) id = GameObject::nextID++;

        shared_ptr<GameObject> obj = make_shared<GameObject>(
            true,
            name,
            id,
            transform,
            mesh,
            assimpMeshes,
            mat);

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
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return obj;
        }

        obj->SetDirectory(modelPath.substr(0, modelPath.find_last_of('/')));

        //process assimps root node recursively
        ProcessNode(obj, scene->mRootNode, scene);

        GameObjectManager::AddGameObject(obj);
        GameObjectManager::AddOpaqueObject(obj);

        //cout << "Successfully initialized " << obj->GetName() << " with ID " << to_string(obj->GetID()) << "\n";

        return obj;
    }

    void Model::ProcessNode(
        shared_ptr<GameObject>& obj,
        aiNode* node,
        const aiScene* scene)
    {
        //process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            //the node object only contains indices to index the actual objects in the scene. 
            //the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            AssimpMesh newMesh = ProcessMesh(obj, mesh, scene);
            obj->AddAssimpMesh(newMesh);

            string objName = obj->GetName();

            shared_ptr<GameObject> newChild = ModelChild::Initialize(
                vec3(0),
                vec3(0),
                vec3(1),
                Engine::filesPath + "/shaders/GameObject.vert",
                Engine::filesPath + "/shaders/GameObject.frag",
                Engine::filesPath + "/textures/default_diffuse.png",
                "EMPTY",
                "EMPTY",
                "EMPTY",
                newMesh.vertices,
                newMesh.indices,
                32,
                objName,
                ModelChild::tempID);

            obj->AddChild(obj, newChild);
        }

        //after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(obj, node->mChildren[i], scene);
        }
    }

    AssimpMesh Model::ProcessMesh(
        shared_ptr<GameObject>& obj,
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