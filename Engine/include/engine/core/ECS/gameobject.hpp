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

#pragma once

//external
#include "glm.hpp"
#include "glfw3.h"

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <string>

using std::string;
using std::shared_ptr;
using std::unordered_map;
using std::type_index;
using std::dynamic_pointer_cast;
using glm::vec2;
using glm::vec3;
using glm::mat4;

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
		enum Type
		{
			cube,
			sphere,
			cone,
			cylinder,
			pyramid,
			point_light,
			spot_light
		};
		static inline string name;
		static inline Type objType;
		static unsigned int nextID;
		unsigned int ID;

		GameObject();

		GameObject(unsigned int GameObjectID);
		unsigned int GetID() const;

		template <typename T>
		static void AddComponent(shared_ptr<T> component);

		template <typename T>
		shared_ptr<T> GetComponent() const;

		static void AddMeshComponent(const float* vertices);

		static void AddMaterialComponent(const vec3& color, float shininess, GLuint VAO, GLuint VBO);
	private:
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
		Mesh(const float* vertices);
		const float* GetVertices() const;
	private:
		const float* vertices;
	};

	class Material : public Component
	{
	public:
		Material(const vec3& color, float shininess, GLuint VAO, GLuint VBO);
		const vec3& GetColor() const;
		float GetShininess() const;
		GLuint GetVAO() const;
		GLuint GetVBO() const;
	private:
		vec3 color;
		float shininess;
		GLuint VAO;
		GLuint VBO;
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