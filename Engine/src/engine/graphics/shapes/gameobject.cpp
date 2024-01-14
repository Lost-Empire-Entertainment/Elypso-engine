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

//engine
#include "gameobject.hpp"

using Type = Graphics::Shape::Mesh::Type;

namespace Graphics::Shape
{
	//
	// TRANSFORM
	//

	Transform::Transform(
		const vec3& pos, 
		const vec3& rot, 
		const vec3& scale) :
		pos(pos), 
		rot(rot), 
		scale(scale)
	{
	}

	void Transform::SetPosition(
		const vec3& assignedPosition)
	{
		pos = assignedPosition;
	}
	void Transform::SetRotation(
		const vec3& assignedRotation)
	{
		rot = assignedRotation;
	}
	void Transform::SetScale(
		const vec3& assignedScale)
	{
		scale = assignedScale;
	}

	vec3 Transform::GetPosition() const
	{
		return pos;
	}
	vec3 Transform::GetRotation() const
	{
		return rot;
	}
	vec3 Transform::GetScale() const
	{
		return scale;
	}

	//
	// MESH
	//

	Mesh::Mesh(
		const Type& type,
		const float* vertices) :
		type(type)
	{
	}

	void Mesh::SetType(Type assignedType)
	{
		type = assignedType;
	}

	Type Mesh::GetType() const
	{
		return type;
	}

	//
	// MATERIAL
	//

	Material::Material(
		const Shader& shader) :
		shader(shader)
	{
	}

	void Material::SetShader(Shader assignedShader)
	{
		shader = assignedShader;
	}

	Shader Material::GetShader() const
	{
		return shader;
	}


	//
	// GAMEOBJECT
	//

	GameObject::GameObject() : 
		ID(0),
		name("None"),
		transform(vec3(0), vec3(0), vec3(0)),
		mesh(Mesh(Type::cube, nullptr)),
		mat(Material(Shader()))
	{
	}

	GameObject::GameObject(
		const unsigned int& ID,
		const string& name,
		const Transform& transform,
		const Mesh& mesh,
		const Material& mat) :
		ID(ID),
		name(name),
		transform(transform),
		mesh(mesh),
		mat(mat)
	{
	}

	void GameObject::Initialize()
	{
		isInitialized = true;
	}
	void GameObject::SetID(const unsigned int& assignedID)
	{
		ID = assignedID;
	}
	void GameObject::SetName(
		const string& assignedName)
	{
		name = assignedName;
	}

	bool GameObject::IsInitialized() const
	{
		return isInitialized;
	}
	unsigned int GameObject::GetID() const
	{
		return ID;
	}
	string GameObject::GetName() const
	{
		return name;
	}
	Transform GameObject::GetTransform() const
	{
		return transform;
	}
	Mesh GameObject::GetMesh() const
	{
		return mesh;
	}
	Material GameObject::GetMaterial() const
	{
		return mat;
	}
}