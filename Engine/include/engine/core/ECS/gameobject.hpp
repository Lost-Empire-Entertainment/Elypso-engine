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

//engine
#include "shader.hpp"

#include <unordered_map>
#include <string>

using std::string;
using glm::vec2;
using glm::vec3;
using glm::mat4;

using Graphics::Shader;

namespace Core::ECS
{
	class GameObject
	{
	public:
		// GAMEOBJECT

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

		static inline unsigned int nextID = 0;

		GameObject();

		GameObject(unsigned int GameObjectID);

		bool IsInitialized() const;
		string GetName() const;
		Type GetType() const;
		unsigned int GetID() const;

		static void Initialize();
		static void SetName(string name);
		static void SetType(Type type);

		// TRANSFORM

		//Transform(const vec3& position, const vec3& rotation, const vec3& scale);

		void SetPosition(const vec3& pos);
		void SetRotation(const vec3& rot);
		void SetScale(const vec3& scale);

		const vec3& GetPosition() const;
		const vec3& GetRotation() const;
		const vec3& GetScale() const;

		// MESH

		//Mesh(const float* vertices);

		void SetVertices(const float* vertices);

		const float* GetVertices() const;

		//MATERIAL

		//Material(const vec3& color, float shininess, GLuint VAO, GLuint VBO, Shader shader);

		void SetVAO(GLuint vao);
		void SetVBO(GLuint vbo);
		void SetShader(Shader shader);

		const vec3& GetColor() const;
		const float GetShininess() const;
		const GLuint GetVAO() const;
		const GLuint GetVBO() const;
		const Shader GetShader() const;
	private:
		// GAMEOBJECT

		static inline bool initialized;
		static inline string name;
		static inline Type type;
		unsigned int ID;

		// TRANSFORM

		vec3 position;
		vec3 rotation;
		vec3 scale;

		// MESH

		const float* vertices;

		// MATERIAL

		vec3 color;
		float shininess;
		GLuint VAO;
		GLuint VBO;
		Shader shader;
	};
}