//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
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

#include "gameobject.hpp"

using std::remove;

using Type = Graphics::Shape::Mesh::MeshType;

namespace Graphics::Shape
{
	//
	// TRANSFORM
	//

	Transform::Transform(
		const vec3& assignedPosition,
		const vec3& assignedRotation,
		const vec3& assignedScale) :
		position(assignedPosition),
		rotation(assignedRotation),
		scale(assignedScale)
	{
		SetPosition(assignedPosition);
		SetRotation(assignedRotation);
		SetScale(assignedScale);
	}

	//
	// MESH
	//

	Mesh::Mesh(const MeshType& assignedType) : type(assignedType)
	{
		SetMeshType(assignedType);
	}

	//
	// MATERIAL
	//

	Material::Material(
		const Shader& assignedShader,
		const GLuint& assignedVAO,
		const GLuint& assignedVBO) :
		shader(assignedShader),
		VAO(assignedVAO),
		VBO(assignedVBO)
	{
		SetShader(assignedShader);
		SetVAO(assignedVAO);
		SetVBO(assignedVBO);
	}

	//
	// GAMEOBJECT
	//

	GameObject::GameObject(
		const bool& assignedIsInitialized,
		const string& assignedName, 
		const unsigned int& assignedID, 
		const shared_ptr<Transform>& assignedTransform,
		const shared_ptr<Mesh>& assignedMesh, 
		const shared_ptr<Material>& assignedMaterial) :
		isInitialized(assignedIsInitialized),
		name(assignedName),
		ID(assignedID),
		transform(assignedTransform),
		mesh(assignedMesh),
		material(assignedMaterial)
	{
		SetName(assignedName);
		SetID(nextID++);
		SetTransform(assignedTransform);
		SetMesh(assignedMesh);
		SetMaterial(assignedMaterial);

		Initialize();
	}

	void GameObject::Render(
		const shared_ptr<GameObject>& obj,
		const mat4& view,
		const mat4& projection) const
	{
		Type meshType = obj->GetMesh()->GetMeshType(); 
		switch (meshType)
		{
		case Type::cube:
			break;
		}
	}

	//
	// GAMEOBJECT MANAGER
	//

	void GameObjectManager::RenderAll(const mat4& view, const mat4& projection) const
	{
		for (const auto& obj : objects)
		{
			shared_ptr<GameObject> object = obj;
			obj->Render(object, view, projection);
		}
	}

	void GameObjectManager::AddGameObject(const shared_ptr<GameObject>& obj)
	{
		objects.push_back(obj);
	}

	void GameObjectManager::DestroyGameObject(const shared_ptr<GameObject>& obj)
	{
		auto it = remove(objects.begin(), objects.end(), obj);
		objects.erase(it, objects.end());
	}
}