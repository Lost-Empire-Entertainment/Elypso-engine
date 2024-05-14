//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <memory>
#include <map>
#include <iostream>

//external
#include "glad.h"

//game
#include "shader.hpp"
#include "gui_node.hpp"
#include "gui_nodeconnection.hpp"

using std::vector;
using std::map;
using std::shared_ptr;
using std::make_shared;
using std::to_string;
using std::cout;
using glm::vec3;
using glm::mat4;

using Graphics::Shader;
using Graphics::GUI::GUINode;
using Graphics::GUI::GUINodeConnection;

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
			scale(scale)
		{
		}

		void SetPosition(const vec3& newPosition)
		{
			position = newPosition;
		}
		void SetRotation(const vec3& newRotation)
		{
			rotation = newRotation;
		}
		void SetScale(const vec3& newScale)
		{
			scale = newScale;
		}

		const vec3& GetPosition() const
		{
			return position;
		}
		const vec3& GetRotation() const
		{
			return rotation;
		}
		const vec3& GetScale() const
		{
			return scale;
		}
	private:
		vec3 position;
		vec3 rotation;
		vec3 scale;
	};

	struct AssimpVertex
	{
		vec3 pos{};
		vec3 normal{};
		vec2 texCoords{};
		vec3 tangent{};
		vec3 bitangent{};
		int boneIDs = 4;
		float weights = 4;
	};

	class AssimpMesh
	{
	public:
		vector<AssimpVertex> vertices;
		vector<unsigned int> indices;

		AssimpMesh(
			vector<AssimpVertex> vertices,
			vector<unsigned int> indices) :
			vertices(vertices),
			indices(indices)
		{
		}
	};

	class Mesh
	{
	public:
		enum class MeshType
		{
			border,
			actionTex,
			model,
			point_light,
			spot_light,
			billboard
		};

		Mesh(const MeshType& type,
			const GLuint& VAO,
			const GLuint& VBO,
			const GLuint& EBO) :
			type(type),
			VAO(VAO),
			VBO(VBO),
			EBO(EBO)
		{
		}
		~Mesh()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}

		void SetMeshType(const MeshType& newType)
		{
			type = newType;
		}
		void SetVAO(const GLuint& newVAO)
		{
			VAO = newVAO;
		}
		void SetVBO(const GLuint& newVBO)
		{
			VBO = newVBO;
		}
		void SetEBO(const GLuint& newEBO)
		{
			EBO = newEBO;
		}
		void SetVertices(const vector<AssimpVertex>& newVertices)
		{
			vertices = newVertices;
		}
		void SetIndices(const vector<unsigned int>& newIndices)
		{
			indices = newIndices;
		}

		const MeshType& GetMeshType() const
		{
			return type;
		}
		const GLuint& GetVAO() const
		{
			return VAO;
		}
		const GLuint& GetVBO() const
		{
			return VBO;
		}
		const GLuint& GetEBO() const
		{
			return EBO;
		}
		const vector<AssimpVertex>& GetVertices() const
		{
			return vertices;
		}
		const vector<unsigned int>& GetIndices() const
		{
			return indices;
		}
	private:
		MeshType type;
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		vector<AssimpVertex> vertices;
		vector<unsigned int> indices;
	};

	class Material
	{
	public:
		enum class TextureType
		{
			diffuse,
			specular,
			normal,
			height
		};

		Material()
		{
		}

		void AddTexture(const string& textureName, const unsigned int& textureID, const TextureType& textureType)
		{
			auto it = textures.find(textureType);
			if (it != textures.end())
			{
				textures.erase(it);
			}

			textures[textureType][textureName] = textureID;
		}

		void AddShader(const string& vertShader, const string& fragShader, const Shader& newShader)
		{
			shaderNames.push_back(vertShader);
			shaderNames.push_back(fragShader);
			shader = newShader;
		}

		const unsigned int& GetTextureID(const TextureType& type) const
		{
			static unsigned int none = 0;

			auto it = textures.find(type);
			if (it != textures.end())
			{
				const auto& textureMap = it->second;
				if (!textureMap.empty())
				{
					return textureMap.begin()->second;
				}
			}
			return none;
		}
		const string& GetTextureName(const TextureType& type) const
		{
			static string none = "";

			auto it = textures.find(type);
			if (it != textures.end())
			{
				const auto& textureMap = it->second;
				if (!textureMap.empty())
				{
					return textureMap.begin()->first;
				}
			}
			return none;
		}
		unsigned int GetTextureCount() const
		{
			unsigned int totalCount = 0;
			for (const auto& pair : textures)
			{
				const auto& textureMap = pair.second;
				totalCount += static_cast<unsigned int>(textureMap.size());
			}
			return totalCount;
		}
		bool TextureExists(const string& texturePath) const
		{
			for (const auto& pair : textures)
			{
				const auto& textureMap = pair.second;
				for (const auto& texture : textureMap)
				{
					if (texture.first == texturePath)
					{
						return true;
					}
				}
			}
			return false;
		}

		const string& GetShaderName(unsigned int position) const
		{
			return shaderNames[position];
		}
		const Shader& GetShader() const
		{
			return shader;
		}
	private:
		map<TextureType, map<string, unsigned int>> textures;
		vector<string> shaderNames;
		Shader shader;
	};

	class BasicShape_Variables
	{
	public:
		BasicShape_Variables(const float& shininess) : shininess(shininess)
		{
		}

		void SetShininess(const float& newShininess)
		{
			shininess = newShininess;
		}

		const float& GetShininess() const
		{
			return shininess;
		}
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
			outerAngle(outerAngle)
		{
		}

		void SetDiffuse(const vec3& newDiffuse)
		{
			diffuse = newDiffuse;
		}
		void SetIntensity(const float& newIntensity)
		{
			intensity = newIntensity;
		}
		void SetDistance(const float& newDistance)
		{
			distance = newDistance;
		}
		void SetInnerAngle(const float& newInnerAngle)
		{
			innerAngle = newInnerAngle;
		}
		void SetOuterAngle(const float& newOuterAngle)
		{
			outerAngle = newOuterAngle;
		}

		const vec3& GetDiffuse() const
		{
			return diffuse;
		}
		const float& GetIntensity() const
		{
			return intensity;
		}
		const float& GetDistance() const
		{
			return distance;
		}
		const float& GetInnerAngle() const
		{
			return innerAngle;
		}
		const float& GetOuterAngle() const
		{
			return outerAngle;
		}
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
			distance(distance)
		{
		}

		void SetDiffuse(const vec3& newDiffuse)
		{
			diffuse = newDiffuse;
		}
		void SetIntensity(const float& newIntensity)
		{
			intensity = newIntensity;
		}
		void SetDistance(const float& newDistance)
		{
			distance = newDistance;
		}

		const vec3& GetDiffuse() const
		{
			return diffuse;
		}
		const float& GetIntensity() const
		{
			return intensity;
		}
		const float& GetDistance() const
		{
			return distance;
		}
	private:
		vec3 diffuse;
		float intensity;
		float distance;
	};

	class Component
	{
	public:
		enum class ComponentType
		{
			Nodeblock
		};

		Component(
			const string& name,
			const ComponentType& type,
			const vector<shared_ptr<GUINode>> nodes) :
			name(name),
			type(type),
			nodes(nodes)
		{
		}

		void AddNode(const shared_ptr<GUINode> newNode)
		{
			nodes.push_back(newNode);
		}

		string GetName() const
		{
			return name;
		}
		ComponentType GetType() const
		{
			return type;
		}
		vector<shared_ptr<GUINode>>& GetNodes()
		{
			return nodes;
		}
		vector<shared_ptr<GUINodeConnection>>& GetNodeConnections()
		{
			return nodeConnections;
		}

	private:
		string name;
		ComponentType type;
		vector<shared_ptr<GUINode>> nodes;
		vector<shared_ptr<GUINodeConnection>> nodeConnections;
	};

	class GameObject
	{
	public:
		static inline unsigned int nextID;

		enum class Category
		{
			cat_Characters_Placeholder,
			cat_Effects_Placeholder,
			cat_UI_Placeholder,
			cat_Lights_Spotlights,
			cat_Lights_Point_lights,
			cat_Props_Static_props,
			cat_All
		};
		static inline vector<Category> categoriesVector =
		{
			Category::cat_Characters_Placeholder,
			Category::cat_Effects_Placeholder,
			Category::cat_UI_Placeholder,
			Category::cat_Lights_Spotlights,
			Category::cat_Lights_Point_lights,
			Category::cat_Props_Static_props,
			Category::cat_All
		};

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
			basicShape(basicShape)
		{
		}

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
			pointLight(pointLight)
		{
		}

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
			spotLight(spotLight)
		{
		}

		//assimp model
		GameObject(
			const bool& isInitialized,
			const string& name,
			const unsigned int& ID,
			const shared_ptr<Transform>& transform,
			const shared_ptr<Mesh>& mesh,
			const vector<AssimpMesh>& assimpMeshes,
			const shared_ptr<Material>& material) :
			isInitialized(isInitialized),
			name(name),
			ID(ID),
			transform(transform),
			mesh(mesh),
			assimpMeshes(assimpMeshes),
			material(material)
		{
		}

		void Initialize() { isInitialized = true; }
		void SetName(const string& newName) { name = newName; }
		void SetID(const unsigned int& newID) { ID = newID; }

		void AddComponent(const shared_ptr<Component> newComponent) { components.push_back(newComponent); }
		void RemoveComponent(const shared_ptr<Component> removedComponent)
		{
			components.erase(remove(components.begin(), components.end(), removedComponent), components.end());
		}
		void SetTransform(const shared_ptr<Transform>& newTransform) { transform = newTransform; }
		void SetMesh(const shared_ptr<Mesh>& newMesh) { mesh = newMesh; }
		void AddAssimpMesh(const AssimpMesh& newMesh) { assimpMeshes.push_back(newMesh); }
		void SetMaterial(const shared_ptr<Material>& newMaterial) { material = newMaterial; }
		void SetBasicShape(const shared_ptr<BasicShape_Variables>& newBasicShape)
		{
			basicShape = newBasicShape;
		}
		void SetPointLight(const shared_ptr<PointLight_Variables>& newPointLight)
		{
			pointLight = newPointLight;
		}
		void SetSpotLight(const shared_ptr<SpotLight_Variables>& newSpotLight)
		{
			spotLight = newSpotLight;
		}

		void SetParent(const shared_ptr<GameObject>& newParent) { parent = newParent; }
		void RemoveParent(const shared_ptr<GameObject>) { parent = nullptr; }
		void AddChild(const shared_ptr<GameObject>& target, const shared_ptr<GameObject>& addedChild)
		{
			children.push_back(addedChild);
			addedChild->SetParent(target);
		}
		void RemoveChild(const shared_ptr<GameObject>& removedChild)
		{
			children.erase(remove(children.begin(), children.end(), removedChild), children.end());
			removedChild->RemoveParent(removedChild);
		}
		void SetParentBillboardHolder(const shared_ptr<GameObject>& newParentBillboardHolder)
		{
			parentBillboardHolder = newParentBillboardHolder;
		}
		void SetChildBillboard(const shared_ptr<GameObject>& newChildBillboard)
		{
			childBillboard = newChildBillboard;
		}

		void SetDirectory(const string& newDirectory) { directory = newDirectory; }

		void SetCategoryState(const Category& category, const bool& state)
		{
			categories[category] = state;
		}
		void SetCategoriesMap(const map<Category, bool>& newCategories) { categories = newCategories; }

		const bool& IsInitialized() const { return isInitialized; }
		const string& GetName() const { return name; }
		const unsigned int& GetID() const {  return ID; }

		const shared_ptr<Transform>& GetTransform() const { return transform; }
		const shared_ptr<Mesh>& GetMesh() const { return mesh; }
		const vector<AssimpMesh>& GetAssimpMeshes() const { return assimpMeshes; }
		const shared_ptr<Material>& GetMaterial() const { return material; }
		const vector<shared_ptr<Component>> GetComponents() const { return components; }
		const shared_ptr<BasicShape_Variables>& GetBasicShape() const { return basicShape; }
		const shared_ptr<PointLight_Variables>& GetPointLight() const { return pointLight; }
		const shared_ptr<SpotLight_Variables>& GetSpotLight() const { return spotLight; }
		const shared_ptr<GameObject>& GetParent() const { return parent; }
		const vector<shared_ptr<GameObject>>& GetChildren() const { return children; }
		const shared_ptr<GameObject>& GetParentBillboardHolder() const { return parentBillboardHolder; }
		const shared_ptr<GameObject>& GetChildBillboard() const { return childBillboard; }
		const string& GetDirectory() const { return directory; }
		bool GetCategoryState(const Category& categoryName)
		{
			return categories[categoryName];
		}
		map<Category, bool> GetCategories() const { return categories; }
	private:
		bool isInitialized;
		string name;
		unsigned int ID;

		shared_ptr<Transform> transform;
		shared_ptr<Mesh> mesh;
		vector<AssimpMesh> assimpMeshes;
		shared_ptr<Material> material;
		vector<shared_ptr<Component>> components;
		shared_ptr<BasicShape_Variables> basicShape;
		shared_ptr<PointLight_Variables> pointLight;
		shared_ptr<SpotLight_Variables> spotLight;
		shared_ptr<GameObject> parent;
		vector<shared_ptr<GameObject>> children;
		shared_ptr<GameObject> parentBillboardHolder;
		shared_ptr<GameObject> childBillboard;
		string directory;
		map<Category, bool> categories;
	};

	class GameObjectManager
	{
	public:
		static void RenderAll(
			const mat4& view,
			const mat4& projection);

		static void AddGameObject(const shared_ptr<GameObject>& obj)
		{
			objects.push_back(obj);
		}
		static void AddOpaqueObject(const shared_ptr<GameObject>& obj)
		{
			opaqueObjects.push_back(obj);
		}
		static void AddTransparentObject(const shared_ptr<GameObject>& obj)
		{
			transparentObjects.push_back(obj);
		}
		static void AddPointLight(const shared_ptr<GameObject>& obj)
		{
			pointLights.push_back(obj);
		}
		static void AddSpotLight(const shared_ptr<GameObject>& obj)
		{
			spotLights.push_back(obj);
		}
		static void AddBillboard(const shared_ptr<GameObject>& obj)
		{
			billboards.push_back(obj);
		}

		static void DestroyGameObject(const shared_ptr<GameObject>& obj);

		static vector<shared_ptr<GameObject>>& GetObjects()
		{
			return objects;
		}
		static vector<shared_ptr<GameObject>> GetPointLights()
		{
			return pointLights;
		}
		static vector<shared_ptr<GameObject>> GetSpotLights()
		{
			return spotLights;
		}
		static vector<shared_ptr<GameObject>> GetBillboards()
		{
			return billboards;
		}
	private:
		static inline vector<shared_ptr<GameObject>> objects;
		static inline vector<shared_ptr<GameObject>> opaqueObjects;
		static inline vector<shared_ptr<GameObject>> transparentObjects;
		static inline vector<shared_ptr<GameObject>> pointLights;
		static inline vector<shared_ptr<GameObject>> spotLights;
		static inline vector<shared_ptr<GameObject>> billboards;
	};
}