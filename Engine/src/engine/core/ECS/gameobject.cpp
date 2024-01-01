//<Elypso engine>
//    Copyright(C) < 2023 > < Greenlaser >
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

using std::make_shared;

namespace Core::ECS
{
	//
	// GAMEOBJECT
	//

	unsigned int GameObject::nextID = 1;
	unordered_map<type_index, shared_ptr<Component>> GameObject::components;

	GameObject::GameObject()
		: ID(nextID++)
	{}

	GameObject::GameObject(unsigned int GameObjectID) 
		: ID(GameObjectID)
	{}

	unsigned int GameObject::GetID() const
	{
		return ID;
	}

	void GameObject::AddMeshComponent(const vector<vec3>& vertices, const vector<unsigned int>& indices, const vector<vec2>& texCoords)
	{
		auto mesh = make_shared<Mesh>(vertices, indices, texCoords);
		AddComponent(mesh);
	}

	void GameObject::AddMaterialComponent(const vec3& color, float shininess)
	{
		auto material = make_shared<Material>(color, shininess);
		AddComponent(material);
	}

	//
	// CREATE CUBE
	//

	GameObject GameObject::CreateCube(const vec3& position, const vec3& scale, const vec3& color, float shininess)
	{
		GameObject cubeObject;
		cubeObject.ID = nextID++;

		auto transform = make_shared<Transform>();
		transform->position = position;
		transform->scale = scale;
		AddComponent(transform);

		vector<vec3> cubeVertices =
		{
			//front face
			vec3(-0.5f, -0.5f, 0.5f),
			vec3(0.5f, -0.5f, 0.5f),
			vec3(0.5f, 0.5f, 0.5f),
			vec3(-0.5f, 0.5f, 0.5f),

			//back face
			vec3(-0.5f, -0.5f, -0.5f),
			vec3(0.5f, -0.5f, -0.5f),
			vec3(0.5f, 0.5f, -0.5f),
			vec3(-0.5f, 0.5f, -0.5f),
		};

		vector<unsigned int> cubeIndices =
		{
			//front face
			0, 1, 2,
			2, 3, 0,

			//right face
			1, 5, 6,
			6, 2, 1,

			//back face
			7, 6, 5,
			5, 4, 7,

			//left face
			4, 0, 3,
			3, 7, 4,

			//top face
			3, 2, 6,
			6, 7, 3,

			//bottom face
			4, 5, 1,
			1, 0, 4
		};

		vector<vec2> cubeTexCoords =
		{
			//front face
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},

			//back face
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},

			//right face
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},

			// Left face
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},

			//top face
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},

			//bottom face
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};

		AddMeshComponent(cubeVertices, cubeIndices, cubeTexCoords);

		auto material = make_shared<Material>(color, shininess);
		AddMaterialComponent(material->GetColor(), material->GetShininess());

		return cubeObject;
	}

	//
	// MESH
	//

	template shared_ptr<Component> GameObject::GetComponent() const;

	Mesh::Mesh(const vector<vec3>& vertices, const vector<unsigned int>& indices, const vector<vec2>& texCoords)
		: vertices(vertices), indices(indices), texCoords(texCoords)
	{}

	const vector<vec3>& Mesh::GetVertices() const
	{
		return vertices;
	}

	const vector<unsigned int>& Mesh::GetIndices() const
	{
		return indices;
	}

	const vector<vec2>& Mesh::GetTexCoords() const
	{
		return texCoords;
	}

	//
	// MATERIAL
	//

	Material::Material(const vec3& color, float shininess)
		: color(color), shininess(shininess)
	{}

	const vec3& Material::GetColor() const
	{
		return color;
	}

	float Material::GetShininess() const
	{
		return shininess;
	}
}