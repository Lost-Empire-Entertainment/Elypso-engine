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

//external
#include "glad.h"

//engine
#include "gameobject.hpp"

using std::make_shared;

using Core::ECS::GameObject;

namespace Core::ECS
{
	//
	// GAMEOBJECT
	//

	unordered_map<type_index, shared_ptr<Component>> GameObject::components;

	GameObject::GameObject()
		: ID(nextID++)
	{}

	GameObject::GameObject(unsigned int GameObjectID) 
		: ID(GameObjectID)
	{}

	// GET VARIABLE

	bool GameObject::IsInitialized() const
	{
		return initialized;
	}

	string GameObject::GetName() const
	{
		return name;
	}

	GameObject::Type GameObject::GetType() const
	{
		return type;
	}

	unsigned int GameObject::GetID() const
	{
		return ID;
	}

	// SET VARIABLE

	void GameObject::Initialize()
	{
		initialized = true;
	}

	void GameObject::SetName(string selectedName)
	{
		name = selectedName;
	}

	void GameObject::SetType(Type selectedType)
	{
		type = selectedType;
	}

	// ADD COMPONENT

	void GameObject::AddMeshComponent(const float* vertices)
	{
		auto mesh = make_shared<Mesh>(vertices);
		AddComponent(mesh);
	}

	void GameObject::AddMaterialComponent(const vec3& color, float shininess, GLuint VAO, GLuint VBO)
	{
		auto material = make_shared<Material>(color, shininess, VAO, VBO);
		AddComponent(material);
	}

	//
	// MESH
	//

	template shared_ptr<Component> GameObject::GetComponent() const;

	Mesh::Mesh(const float* vertices)
		: vertices(vertices)
	{}

	const float* Mesh::GetVertices() const
	{
		return vertices;
	}

	//
	// MATERIAL
	//

	Material::Material(const vec3& color, float shininess, GLuint VAO, GLuint VBO)
		: color(color), shininess(shininess), VAO(VAO), VBO(VBO)
	{}

	const vec3& Material::GetColor() const
	{
		return color;
	}

	float Material::GetShininess() const
	{
		return shininess;
	}

	GLuint Material::GetVAO() const
	{
		return VAO;
	}

	GLuint Material::GetVBO() const
	{
		return VBO;
	}
}