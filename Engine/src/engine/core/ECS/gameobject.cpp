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

using Core::ECS::GameObject;

namespace Core::ECS
{
	//
	// GAMEOBJECT
	//

	GameObject::GameObject()
		: ID(nextID++)
	{}

	GameObject::GameObject(
		unsigned int assignedID) : 
			ID(assignedID)
	{}

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

	void GameObject::SetPosition(const vec3& assignedPosition)
	{
		GameObject::position = assignedPosition;
	}
	void GameObject::SetRotation(const vec3& assignedRotation)
	{
		GameObject::rotation = assignedRotation;
	}
	void GameObject::SetScale(const vec3& assignedScale)
	{
		GameObject::scale = assignedScale;
	}

	const vec3& GameObject::GetPosition() const
	{
		return GameObject::position;
	}
	const vec3& GameObject::GetRotation() const
	{
		return GameObject::rotation;
	}
	const vec3& GameObject::GetScale() const
	{
		return GameObject::scale;
	}

	//
	// MESH
	//

	void GameObject::SetVertices(const float* assignedVertices)
	{
		GameObject::vertices = assignedVertices;
	}

	const float* GameObject::GetVertices() const
	{
		return GameObject::vertices;
	}

	//
	// MATERIAL
	//

	void GameObject::SetVAO(GLuint assignedVAO)
	{
		GameObject::VAO = assignedVAO;
	}
	void GameObject::SetVBO(GLuint assignedVBO)
	{
		GameObject::VBO = assignedVBO;
	}
	void GameObject::SetShader(Shader assignedShader)
	{
		GameObject::shader = assignedShader;
	}

	const vec3& GameObject::GetColor() const
	{
		return GameObject::color;
	}
	const float GameObject::GetShininess() const
	{
		return GameObject::shininess;
	}
	const GLuint GameObject::GetVAO() const
	{
		return GameObject::VAO;
	}
	const GLuint GameObject::GetVBO() const
	{
		return GameObject::VBO;
	}
	const Shader GameObject::GetShader() const
	{
		return GameObject::shader;
	}
}