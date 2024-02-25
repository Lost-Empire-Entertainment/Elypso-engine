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

#include <vector>
#include <memory>

//external
#include "glad.h"

//engine
#include "shader.hpp"

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
			const vec3& scale) :
			position(position),
			rotation(rotation),
			scale(scale) {}

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
			border,
			actionTex,
			cube,
			point_light,
			spot_light,
			billboard
		};

		Mesh(const MeshType& type) : type(type) {}

		void SetMeshType(const MeshType& newType) { type = newType; }

		const MeshType& GetMeshType() const { return type; }
	private:
		MeshType type;
	};

	class Material
	{
	public:
		Material(
			const GLuint& VAO,
			const GLuint& VBO) :
			VAO(VAO),
			VBO(VBO) {}
		~Material()
		{
			unsigned int shaderID = GetShader().ID;
			glDeleteShader(shaderID);
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}

		void SetVAO(const GLuint& newVAO) { VAO = newVAO; }
		void SetVBO(const GLuint& newVBO) { VBO = newVBO; }
		void AddTexture(const string& newTextureName, const unsigned int& newTextureID)
		{
			textureNames.push_back(newTextureName);
			textureIDs.push_back(newTextureID);
		}
		void AddShader(const string& vertShader, const string& fragShader, const Shader& newShader)
		{
			shaderNames.push_back(vertShader);
			shaderNames.push_back(fragShader);
			shader = newShader;
		}

		const GLuint& GetVAO() const { return VAO; }
		const GLuint& GetVBO() const { return VBO; }
		const unsigned int& GetTextureID(unsigned int position) const { return textureIDs[position]; }
		const string& GetTextureName(unsigned int position) const { return textureNames[position]; }
		const string& GetShaderName(unsigned int position) const { return shaderNames[position]; }
		const Shader& GetShader() const { return shader; }
	private:
		vector<string> textureNames;
		vector<unsigned int> textureIDs;
		vector<string> shaderNames;
		Shader shader;
		GLuint VAO;
		GLuint VBO;
	};

	class BasicShape_Variables
	{
	public:
		BasicShape_Variables(const float& shininess) : shininess(shininess) {}

		void SetShininess(const float& newShininess) { shininess = newShininess; }

		const float& GetShininess() const { return shininess; }
	private:
		float shininess;
	};

	class SpotLight_Variables
	{
	public:
		SpotLight_Variables(
			const vec3& diffuse,
			const float& intensity,
			const float& distance,
			const float& innerAngle,
			const float& outerAngle) :
			diffuse(diffuse),
			intensity(intensity),
			distance(distance),
			innerAngle(innerAngle),
			outerAngle(outerAngle) {}

		void SetDiffuse(const vec3& newDiffuse) { diffuse = newDiffuse; }
		void SetIntensity(const float& newIntensity) { intensity = newIntensity; }
		void SetDistance(const float& newDistance) { distance = newDistance; }
		void SetInnerAngle(const float& newInnerAngle) { innerAngle = newInnerAngle; }
		void SetOuterAngle(const float& newOuterAngle) { outerAngle = newOuterAngle; }

		const vec3& GetDiffuse() const { return diffuse; }
		const float& GetIntensity() const { return intensity; }
		const float& GetDistance() const { return distance; }
		const float& GetInnerAngle() const { return innerAngle; }
		const float& GetOuterAngle() const { return outerAngle; }
	private:
		vec3 diffuse;
		float intensity;
		float distance;
		float innerAngle;
		float outerAngle;
	};

	class PointLight_Variables
	{
	public:
		PointLight_Variables(
			const vec3& diffuse,
			const float& intensity,
			const float& distance) :
			diffuse(diffuse),
			intensity(intensity),
			distance(distance) {}

		void SetDiffuse(const vec3& newDiffuse) { diffuse = newDiffuse; }
		void SetIntensity(const float& newIntensity) { intensity = newIntensity; }
		void SetDistance(const float& newDistance) { distance = newDistance; }

		const vec3& GetDiffuse() const { return diffuse; }
		const float& GetIntensity() const { return intensity; }
		const float& GetDistance() const { return distance; }
	private:
		vec3 diffuse;
		float intensity;
		float distance;
	};

	class GameObject
	{
	public:
		static inline unsigned int nextID;

		//basic gameobject
		GameObject(
			const bool& isInitialized,
			const string& name,
			const unsigned int& ID,
			const shared_ptr<Transform>& transform,
			const shared_ptr<Mesh>& mesh,
			const shared_ptr<Material>& material,
			const shared_ptr<BasicShape_Variables>& basicShape) :
			isInitialized(isInitialized),
			name(name),
			ID(ID),
			transform(transform),
			mesh(mesh),
			material(material),
			basicShape(basicShape) {}

		//point light
		GameObject(
			const bool& isInitialized,
			const string& name,
			const unsigned int& ID,
			const shared_ptr<Transform>& transform,
			const shared_ptr<Mesh>& mesh,
			const shared_ptr<Material>& material,
			const shared_ptr<PointLight_Variables>& pointLight) :
			isInitialized(isInitialized),
			name(name),
			ID(ID),
			transform(transform),
			mesh(mesh),
			material(material),
			pointLight(pointLight) {}

		//spotlight
		GameObject(
			const bool& isInitialized,
			const string& name,
			const unsigned int& ID,
			const shared_ptr<Transform>& transform,
			const shared_ptr<Mesh>& mesh,
			const shared_ptr<Material>& material,
			const shared_ptr<SpotLight_Variables>& spotLight) :
			isInitialized(isInitialized),
			name(name),
			ID(ID),
			transform(transform),
			mesh(mesh),
			material(material),
			spotLight(spotLight) {}

		void Initialize() { isInitialized = true; }
		void SetName(const string& newName) { name = newName; }
		void SetID(const unsigned int& newID) { ID = newID; }

		void SetTransform(const shared_ptr<Transform>& newTransform) { transform = newTransform; }
		void SetMesh(const shared_ptr<Mesh>& newMesh) { mesh = newMesh; }
		void SetMaterial(const shared_ptr<Material>& newMaterial) { material = newMaterial; }
		void SetBasicShape(const shared_ptr<BasicShape_Variables>& newBasicShape) { basicShape = newBasicShape; }
		void SetPointLight(const shared_ptr<PointLight_Variables>& newPointLight) { pointLight = newPointLight; }
		void SetSpotLight(const shared_ptr<SpotLight_Variables>& newSpotLight) { spotLight = newSpotLight; }

		void SetParent(const shared_ptr<GameObject>& newParent) { parent = newParent; }
		void SetChild(const shared_ptr<GameObject>& newChild) { child = newChild; }
		void SetParentBillboardHolder(const shared_ptr<GameObject>& newParentBillboardHolder) { parentBillboardHolder = newParentBillboardHolder; }
		void SetChildBillboard(const shared_ptr<GameObject>& newChildBillboard) { childBillboard = newChildBillboard; }

		const bool& IsInitialized() const { return isInitialized; }
		const string& GetName() const { return name; }
		const unsigned int& GetID() const { return ID; }

		const shared_ptr<Transform>& GetTransform() const { return transform; }
		const shared_ptr<Mesh>& GetMesh() const { return mesh; }
		const shared_ptr<Material>& GetMaterial() const { return material; }
		const shared_ptr<BasicShape_Variables>& GetBasicShape() const { return basicShape; }
		const shared_ptr<PointLight_Variables>& GetPointLight() const { return pointLight; }
		const shared_ptr<SpotLight_Variables>& GetSpotLight() const { return spotLight; }
		const shared_ptr<GameObject>& GetParent() const { return parent; }
		const shared_ptr<GameObject>& GetChild() const { return child; }
		const shared_ptr<GameObject>& GetParentBillboardHolder() const { return parentBillboardHolder; }
		const shared_ptr<GameObject>& GetChildBillboard() const { return childBillboard; }
	private:
		bool isInitialized;
		string name;
		unsigned int ID;

		shared_ptr<Transform> transform;
		shared_ptr<Mesh> mesh;
		shared_ptr<Material> material;
		shared_ptr<BasicShape_Variables> basicShape;
		shared_ptr<PointLight_Variables> pointLight;
		shared_ptr<SpotLight_Variables> spotLight;
		shared_ptr<GameObject> parent;
		shared_ptr<GameObject> child;
		shared_ptr<GameObject> parentBillboardHolder;
		shared_ptr<GameObject> childBillboard;
	};

	class GameObjectManager
	{
	public:
		static void RenderAll(
			const mat4& view,
			const mat4& projection);

		static void AddGameObject(const shared_ptr<GameObject>& obj) { objects.push_back(obj); }
		static void AddOpaqueObject(const shared_ptr<GameObject>& obj) { opaqueObjects.push_back(obj); }
		static void AddTransparentObject(const shared_ptr<GameObject>& obj) { transparentObjects.push_back(obj); }
		static void AddPointLight(const shared_ptr<GameObject>& obj) { pointLights.push_back(obj); }
		static void AddSpotLight(const shared_ptr<GameObject>& obj) { spotLights.push_back(obj); }
		static void SetActionTex(const shared_ptr<GameObject>& newActionTex) { actionTex = newActionTex; }
		static void SetBorder(const shared_ptr<GameObject>& newBorder) { border = newBorder; }
		static void AddBillboard(const shared_ptr<GameObject>& obj) { billboards.push_back(obj); }

		static void DestroyGameObject(const shared_ptr<GameObject>& obj);

		static vector<shared_ptr<GameObject>>& GetObjects() { return objects; }
		static vector<shared_ptr<GameObject>> GetPointLights() { return pointLights; }
		static vector<shared_ptr<GameObject>> GetSpotLights() { return spotLights; }
		static shared_ptr<GameObject> GetActionTex() { return actionTex; }
		static shared_ptr<GameObject> GetBorder() { return border; }
		static vector<shared_ptr<GameObject>> GetBillboards() { return billboards; }
	private:
		static inline vector<shared_ptr<GameObject>> objects;
		static inline vector<shared_ptr<GameObject>> opaqueObjects;
		static inline vector<shared_ptr<GameObject>> transparentObjects;
		static inline vector<shared_ptr<GameObject>> pointLights;
		static inline vector<shared_ptr<GameObject>> spotLights;
		static inline shared_ptr<GameObject> actionTex;
		static inline shared_ptr<GameObject> border;
		static inline vector<shared_ptr<GameObject>> billboards;
	};
}