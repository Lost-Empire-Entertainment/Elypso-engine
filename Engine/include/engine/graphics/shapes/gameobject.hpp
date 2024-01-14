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

//engine
#include "shader.hpp"
#include "render.hpp"

#include <string>
#include <vector>

using std::string;
using std::vector;
using glm::vec3;
using glm::mat4;

using Graphics::Shader;

namespace Graphics::Shape
{
	class Transform
	{
	public:
		Transform(
			const vec3& pos, 
			const vec3& rot, 
			const vec3& scale);

		void SetPosition(const vec3& pos);
		void SetRotation(const vec3& rot);
		void SetScale(const vec3& scale);

		vec3 GetPosition() const;
		vec3 GetRotation() const;
		vec3 GetScale() const;
	private:
		vec3 pos;
		vec3 rot;
		vec3 scale;
	};

	class Mesh
	{
	public:
		enum Type
		{
			// BASE SHAPES

			cube,
			sphere,
			cylinder,
			pyramid,
			cone,

			// LIGHT SOURCES

			point_light,
			spotlight

		};
		Mesh(
			const Type& type,
			const float* vertices);

		void SetType(Type type);

		Type GetType() const;
	private:
		Type type;
	};

	class Material
	{
	public:
		Material(const Shader& shader);

		void SetShader(Shader shader);

		Shader GetShader() const;
	private:
		Shader shader;
	};

	class GameObject
	{
	public:
		static inline vector<GameObject> gameObjects;
		static inline vector<GameObject> pointLights;
		static inline vector<GameObject> spotLights;

		static inline int nextID;

		GameObject();

		GameObject(
			const unsigned int& ID,
			const string& name,
			const Transform& transform, 
			const Mesh& mesh,
			const Material& material);

		void Initialize();
		void SetID(const unsigned int& ID);
		void SetName(const string& name);

		bool IsInitialized() const;
		unsigned int GetID() const;
		string GetName() const;
		Transform GetTransform() const;
		Mesh GetMesh() const;
		Material GetMaterial() const;
	private:
		bool isInitialized;
		unsigned int ID;
		string name;
		Transform transform;
		Mesh mesh;
		Material mat;
	};
}