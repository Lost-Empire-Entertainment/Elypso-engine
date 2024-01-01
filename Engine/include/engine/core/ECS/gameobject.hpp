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

#pragma once

//external
#include "glm.hpp"

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::unordered_map;
using std::type_index;
using std::dynamic_pointer_cast;
using std::vector;

using glm::vec2;
using glm::vec3;

namespace Core::ECS
{
	class Component
	{
	public:
		virtual ~Component() 
		{}
	};

	class GameObject
	{
	public:
		GameObject();

		GameObject(unsigned int GameObjectID);
		unsigned int GetID() const;

		template <typename T>
		static void AddComponent(shared_ptr<T> component);

		template <typename T>
		shared_ptr<T> GetComponent() const;

		static void AddMeshComponent(const vector<vec3>& vertices, const vector<unsigned int>& indices, const vector<vec2>& texCoords);

		static void AddMaterialComponent(const vec3& color, float shininess);

		static GameObject CreateCube(const vec3& position, const vec3& scale, const vec3& color, float shininess);
	private:
		static unsigned int nextID;
		unsigned int ID;
		static unordered_map<type_index, shared_ptr<Component>> components;
	};

	class Transform : public Component
	{
	public:
		vec3 position;
		vec3 rotation;
		vec3 scale;
	};

	class Mesh : public Component
	{
	public:
		Mesh(const vector<vec3>& vertices, const vector<unsigned int>& indices, const vector<vec2>& texCoords);
		const vector<vec3>& GetVertices() const;
		const vector<unsigned int>& GetIndices() const;
		const vector<vec2>& GetTexCoords() const;
	private:
		vector<vec3> vertices;
		vector<unsigned int> indices;
		vector<vec2> texCoords;
	};

	class Material : public Component
	{
	public:
		Material(const vec3& color, float shininess);
		const vec3& GetColor() const;
		float GetShininess() const;
	private:
		vec3 color;
		float shininess;
	};
}

template <typename T>
void Core::ECS::GameObject::AddComponent(shared_ptr<T> component)
{
	components[type_index(typeid(T))] = component;
}

template <typename T>
shared_ptr<T> Core::ECS::GameObject::GetComponent() const
{
	auto it = components.find(type_index(typeid(T)));
	if (it != components.end())
	{
		return dynamic_pointer_cast<T>(it->second);
	}
	else
	{
		return nullptr;
	}
}