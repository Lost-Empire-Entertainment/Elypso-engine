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

	//
	// TRANSFORM
	//

	Transform::Transform(const vec3& initPos, const vec3& initRot, const vec3& initScale)
		: position(initPos), rotation(initRot), scale(initScale)
	{}

	void Transform::SetPosition(const vec3& pos)
	{
		Transform::position = pos;
	}
	void Transform::SetRotation(const vec3& rot)
	{
		Transform::rotation = rot;
	}
	void Transform::SetScale(const vec3& scale)
	{
		Transform::scale = scale;
	}

	const vec3& Transform::GetPosition() const
	{
		return Transform::position;
	}
	const vec3& Transform::GetRotation() const
	{
		return Transform::rotation;
	}
	const vec3& Transform::GetScale() const
	{
		return Transform::scale;
	}

	//
	// MESH
	//

	Mesh::Mesh(const float* vertices)
		: vertices(vertices)
	{}

	void Mesh::SetVertices(const float* assignedVertices)
	{
		vertices = assignedVertices;
	}

	const float* Mesh::GetVertices() const
	{
		return vertices;
	}

	//
	// MATERIAL
	//

	Material::Material(const vec3& color, float shininess, GLuint VAO, GLuint VBO, Shader shader)
		: color(color), shininess(shininess), VAO(VAO), VBO(VBO), shader(shader)
	{}

	void Material::SetVAO(GLuint vao)
	{
		VAO = vao;
	}
	void Material::SetVBO(GLuint vbo)
	{
		VBO = vbo;
	}
	void Material::SetShader(Shader shader)
	{
		this->shader = shader;
	}

	const vec3& Material::GetColor() const
	{
		return color;
	}
	const float Material::GetShininess() const
	{
		return shininess;
	}
	const GLuint Material::GetVAO() const
	{
		return VAO;
	}
	const GLuint Material::GetVBO() const
	{
		return VBO;
	}
	const Shader Material::GetShader() const
	{
		return shader;
	}
}