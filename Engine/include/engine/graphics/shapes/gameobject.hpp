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

#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;
using std::make_shared;
using glm::vec3;
using glm::mat4;

using Graphics::Shader;

namespace Graphics::Shape
{
	class Transform
	{
	public:
		Transform(
			const vec3& position, 
			const vec3& rotation, 
			const vec3& scale);

		void SetPosition(const vec3& newPosition) { position = newPosition; }
		void SetRotation(const vec3& newRotation) { rotation = newRotation; }
		void SetScale(const vec3& newScale) { scale = newScale; }

		const vec3& GetPosition() const { return position; }
		const vec3& GetRotation() const { return rotation; }
		const vec3& GetScale() const { return scale; }
	private:
		vec3 position;
		vec3 rotation;
		vec3 scale;
	};

	class Mesh
	{
	public:
		enum class MeshType
		{
			cube,
			sphere,
			cylinder,
			cone,
			pyramid,
			assimp,
			point_light,
			spot_light,
			directional_light
		};

		Mesh(const MeshType& type);

		void SetMeshType(const MeshType& newType) { type = newType; }

		const MeshType& GetMeshType() const { return type; }
	private:
		MeshType type;
	};

	class Material
	{
	public:
		Material(
			const Shader& shader, 
			const GLuint& VAO,
			const GLuint& VBO);

		void SetShader(const Shader& newShader) { shader = newShader; }
		void SetVAO(const GLuint& newVAO) { VAO = newVAO; }
		void SetVBO(const GLuint& newVBO) { VBO = newVBO; }

		const Shader& GetShader() const { return shader; }
		const GLuint& GetVAO() const { return VAO; }
		const GLuint& GetVBO() const { return VBO; }
	private:
		Shader shader;
		GLuint VAO;
		GLuint VBO;
	};

	class GameObject;

	class GameObjectManager
	{
	public:
		void RenderAll(
			const mat4& view, 
			const mat4& projection) const;

		void AddGameObject(const shared_ptr<GameObject>& obj);
		void DestroyGameObject(const shared_ptr<GameObject>& obj);

		const vector<shared_ptr<GameObject>>& GetObjects() const { return objects; }
		const vector<shared_ptr<GameObject>> GetPointLights() const { return pointLights; }
	private:
		vector<shared_ptr<GameObject>> objects;
		vector<shared_ptr<GameObject>> pointLights;
	};

	class GameObject
	{
	public:
		static inline unsigned int nextID;

		GameObject(
			const string& assignedName,
			const unsigned int& ID,
			const shared_ptr<Transform>& assignedTransform,
			const shared_ptr<Mesh>& assignedMesh,
			const shared_ptr<Material>& assignedMaterial);

		virtual void Render(
			const GameObject& obj,
			const mat4& view,
			const mat4& projection) const;

		void SetName(const string& newName) { name = newName; }
		void SetID(const unsigned int& newID) { ID = newID; }
		void SetTransform(const shared_ptr<Transform>& newTransform) { transform = newTransform; }
		void SetMesh(const shared_ptr<Mesh>& newMesh) { mesh = newMesh; }
		void SetMaterial(const shared_ptr<Material>& newMaterial) { material = newMaterial; }

		const string& GetName() const { return name; }
		const unsigned int& GetID() const { return ID; }
		const shared_ptr<Transform>& GetTransform() const { return transform; }
		const shared_ptr<Mesh>& GetMesh() const { return mesh; }
		const shared_ptr<Material>& GetMaterial() const { return material; }
	private:
		string name;
		unsigned int ID;
		shared_ptr<Transform> transform;
		shared_ptr<Mesh> mesh;
		shared_ptr<Material> material;
	};
}