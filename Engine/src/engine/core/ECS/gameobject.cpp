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

	bool GameObject::IsInitialized() const
	{
		return GameObject::initialized;
	}
	string GameObject::GetName() const
	{
		return GameObject::name;
	}
	GameObject::Type GameObject::GetType() const
	{
		return GameObject::type;
	}
	unsigned int GameObject::GetID() const
	{
		return GameObject::ID;
	}

	void GameObject::Initialize()
	{
		GameObject::initialized = true;
	}
	void GameObject::SetName(string assignedName)
	{
		GameObject::name = assignedName;
	}
	void GameObject::SetType(Type assignedType)
	{
		GameObject::type = assignedType;
	}

	//
	// TRANSFORM
	//

	Transform::Transform(
		vec3 assignedPosition, 
		vec3 assignedRotation, 
		vec3 assignedScale) : 
			position(assignedPosition),
			rotation(assignedRotation),
			scale(assignedScale)
	{}

	void Transform::SetPosition(vec3 assignedPosition)
	{
		Transform::position = assignedPosition;
	}
	void Transform::SetRotation(vec3 assignedRotation)
	{
		Transform::rotation = assignedRotation;
	}
	void Transform::SetScale(vec3 assignedScale)
	{
		Transform::scale = assignedScale;
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

	Mesh::Mesh(
		const float* assignedVertices) : 
			vertices(assignedVertices)
	{}

	void Mesh::SetVertices(const float* assignedVertices)
	{
		Mesh::vertices = assignedVertices;
	}

	const float* Mesh::GetVertices() const
	{
		return Mesh::vertices;
	}

	//
	// MATERIAL
	//

	Material::Material(
		const vec3& assignedColor, 
		float assignedShininess, 
		GLuint assignedVAO,
		GLuint assignedVBO, 
		Shader assignedShader) : 
			color(assignedColor),
			shininess(assignedShininess),
			VAO(assignedVAO),
			VBO(assignedVBO),
			shader(assignedShader)
	{}

	void Material::SetVAO(GLuint assignedVAO)
	{
		Material::VAO = assignedVAO;
	}
	void Material::SetVBO(GLuint assignedVBO)
	{
		Material::VBO = assignedVBO;
	}
	void Material::SetShader(Shader assignedShader)
	{
		Material::shader = assignedShader;
	}

	const vec3& Material::GetColor() const
	{
		return Material::color;
	}
	const float Material::GetShininess() const
	{
		return Material::shininess;
	}
	const GLuint Material::GetVAO() const
	{
		return Material::VAO;
	}
	const GLuint Material::GetVBO() const
	{
		return Material::VBO;
	}
	const Shader Material::GetShader() const
	{
		return Material::shader;
	}
}