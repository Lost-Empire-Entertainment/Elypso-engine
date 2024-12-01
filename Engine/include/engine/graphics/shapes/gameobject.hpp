//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>

//external
#include "glad.h"
#include "magic_enum.hpp"
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "selectobject.hpp"
#include "shader.hpp"
#include "billboard.hpp"

namespace Graphics::Shape
{
	using std::vector;
	using std::map;
	using std::shared_ptr;
	using std::weak_ptr;
	using std::make_shared;
	using std::cout;
	using glm::vec3;
	using glm::mat4;
	using glm::scale;
	using glm::translate;
	using glm::mat4_cast;
	using glm::quat;
	using std::string;
	using std::enable_shared_from_this;
	using std::to_string;
	using std::filesystem::path;

	using Graphics::Shader;
	using Core::Select;

	class Component
	{
	public:
		virtual ~Component() = default;

		virtual void Initialize(const shared_ptr<GameObject>& parent, const float* vertices)
		{
			this->parent = parent;
		}

		virtual void InitializeModel(const shared_ptr<GameObject>& parent, const vector<AssimpVertex>& vertices)
		{
			this->parent = parent;
		}

		virtual void Update(float deltaTime) {}
		virtual void Render(const mat4& view, const mat4& projection) {}

	protected:
		weak_ptr<GameObject> parent;
	};

	class Transform : public Component
	{
	public:
		Transform(
			const vec3& position,
			const vec3& rotation,
			const vec3& scale) :
			position(position),
			rotation(rotation),
			scale(scale) {}

		void Update(float deltaTime) override {}

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
			indices(indices) {}
	};

	class Mesh : public Component
	{
	public:
		enum class MeshType
		{
			border,
			actionTex,
			model,
			point_light,
			spot_light,
			directional_light,
			billboard,
			skybox
		};

		Mesh(const bool& isEnabled,
			const MeshType& type) :
			isEnabled(isEnabled),
			type(type),
			VAO(0),
			VBO(0),
			EBO(0) {}
		~Mesh()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}

		void Initialize(const MeshType& meshType, const float* vertices, size_t vertexSize)
		{
			switch (meshType)
			{
			case MeshType::border:
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				glBindVertexArray(0);

				break;
			case MeshType::actionTex:
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);

				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
				glEnableVertexAttribArray(2);

				glBindVertexArray(0);

				break;
			case MeshType::skybox:
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

				break;

			case MeshType::billboard:
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);

				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
				glEnableVertexAttribArray(2);

				glBindVertexArray(0);

				break;
			case MeshType::point_light:
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);

				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				glBindVertexArray(0);

				break;
			case MeshType::spot_light:
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);

				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				glBindVertexArray(0);

				break;
			case MeshType::directional_light:
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);

				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				glBindVertexArray(0);

				break;
			}
		}

		void InitializeModel(const MeshType& meshType, const vector<AssimpVertex> assimpVertex)
		{
			size_t verticeSize = sizeof(vertices) * sizeof(assimpVertex);

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, verticeSize, &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			//vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(assimpVertex), (void*)0);
			//vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(assimpVertex), (void*)offsetof(AssimpVertex, normal));
			//vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(assimpVertex), (void*)offsetof(AssimpVertex, texCoords));
			//vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(assimpVertex), (void*)offsetof(AssimpVertex, tangent));
			//vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(assimpVertex), (void*)offsetof(AssimpVertex, bitangent));
			//ids
			glEnableVertexAttribArray(5);
			glVertexAttribIPointer(5, 4, GL_INT, sizeof(assimpVertex), (void*)offsetof(AssimpVertex, boneIDs));
			//weights
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(assimpVertex), (void*)offsetof(AssimpVertex, weights));
			glBindVertexArray(0);
		}

		void SetEnableState(const bool& newIsEnabled)
		{
			isEnabled = newIsEnabled;
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

		const bool& IsEnabled() const
		{
			return isEnabled;
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
		bool isEnabled;
		MeshType type;
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		vector<AssimpVertex> vertices;
		vector<unsigned int> indices;
	};

	class Material : public Component
	{
	public:
		enum class TextureType
		{
			diffuse,
			specular,
			normal,
			height,
			misc_icon_blank,
			misc_icon_move,
			misc_icon_rotate,
			misc_icon_scale
		};

		Material() {}

		void Initialize(const string& vertShaderPath, const string& fragShaderPath)
		{
			Shader newShader = Shader::LoadShader(vertShaderPath, fragShaderPath);
			AddShader(vertShaderPath, fragShaderPath, newShader);

			newShader.Use();
			newShader.SetInt("material.diffuse", 0);
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

	class LightComponent : public Component
	{
	public:
		struct LightConfig 
		{
			vec3 diffuse;
			float intensity;
			string vertShader;
			string fragShader;
			bool isMeshEnabled;
			string billboardVertShader;
			string billboardFragShader;
			string billboardDiffTexture;
			float billboardShininess;
			bool isBillboardEnabled;
		};

		LightComponent(const LightConfig& config) :
			diffuse(config.diffuse),
			intensity(config.intensity),
			vertShader(config.vertShader),
			fragShader(config.fragShader),
			isMeshEnabled(config.isMeshEnabled),
			billboardVertShader(config.billboardVertShader),
			billboardFragShader(config.billboardFragShader),
			billboardDiffTexture(config.billboardDiffTexture),
			billboardShininess(config.billboardShininess),
			isBillboardEnabled(config.isBillboardEnabled) {}

		void Initialize(const shared_ptr<GameObject>& parent, const float* vertices) override
		{
			this->parent = parent;

			auto mesh = parent->AddComponent<Mesh>(isMeshEnabled, Mesh::MeshType::point_light);
			mesh->Initialize(Mesh::MeshType::point_light, vertices, sizeof(vertices));

			auto material = parent->AddComponent<Material>();
			material->Initialize(
				(path(Engine::filesPath) / "shaders" / vertShader).string(),
				(path(Engine::filesPath) / "shaders" / fragShader).string());

			SetupBillboard(parent);
		}

		void Render(const mat4& view, const mat4& projection) override
		{
			auto parentPtr = parent.lock();
			if (!parentPtr 
				|| !parentPtr->IsEnabled())
			{
				return;
			}

			auto material = parentPtr->GetComponent<Material>();
			auto mesh = parentPtr->GetComponent<Mesh>();
			auto& transform = parentPtr->GetTransform();

			if (material 
				&& mesh 
				&& transform)
			{
				Shader shader = material->GetShader();

				CastLight(
					shader, 
					view, 
					projection, 
					GetDiffuse());

				if (GameObjectManager::renderLightBorders 
					&& mesh->IsEnabled())
				{
					RenderBorder(
						transform, 
						shader, 
						mesh->GetVAO(), 
						GetVertexCount());
				}
			}
		}

		void SetDiffuse(const vec3& newDiffuse) { diffuse = newDiffuse; }
		void SetIntensity(const float& newIntensity) { intensity = newIntensity; }

		const vec3& GetDiffuse() const { return diffuse; }
		const float& GetIntensity() const { return intensity; }

	protected:
		virtual int GetVertexCount() const = 0;

		void CastLight(
			Shader& shader, 
			const mat4& view, 
			const mat4& projection, 
			const vec3& lightColor)
		{
			shader.Use();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);
			shader.SetVec3("color", lightColor);

			float transparency = 
				(Select::selectedObj 
					== parent.lock() 
					&& Select::isObjectSelected) 
				? 1.0f 
				: 0.5f;
			shader.SetFloat("transparency", transparency);
		}

		void RenderBorder(
			const shared_ptr<Transform>& transform, 
			Shader& shader, 
			GLuint vao, 
			int vertexCount)
		{
			mat4 model = mat4(1.0f);
			model = translate(model, transform->GetPosition());
			quat newRot = quat(radians(transform->GetRotation()));
			model *= mat4_cast(newRot);
			model = scale(model, transform->GetScale());

			shader.SetMat4("model", model);

			glBindVertexArray(vao);
			glDrawArrays(GL_LINES, 0, vertexCount);
		}

		void SetupBillboard(const shared_ptr<GameObject>& parent) const
		{
			string billboardName = parent->GetName() + "_Billboard";
			auto billboard = Billboard::InitializeBillboard(
				parent->GetTransform()->GetPosition(),
				parent->GetTransform()->GetRotation(),
				parent->GetTransform()->GetScale(),
				billboardVertShader,
				billboardFragShader,
				billboardDiffTexture,
				billboardShininess,
				billboardName,
				++GameObject::nextID,
				isBillboardEnabled);

			billboard->SetParent(parent);
			parent->AddChild(parent, billboard);
		}

		vec3 diffuse;
		float intensity;

		string vertShader;
		string fragShader;
		bool isMeshEnabled;

		string billboardVertShader;
		string billboardFragShader;
		string billboardDiffTexture;
		float billboardShininess;
		bool isBillboardEnabled;

		weak_ptr<GameObject> parent;
	};

	class PointLightComponent : public LightComponent
	{
	public:
		PointLightComponent(
			const LightConfig& config, 
			float distance) : 
			LightComponent(config),
			distance(distance) {}

		void SetDistance(const float& newDistance) { distance = newDistance; }
		const float& GetDistance() const { return distance; }

	protected:
		int GetVertexCount() const override { return 24; }

	private:

		float distance;
	};

	class SpotLightComponent : public LightComponent
	{
	public:
		SpotLightComponent(
			const LightConfig& config,
			float distance,
			float innerAngle,
			float outerAngle) :
			LightComponent(config),
			distance(distance),
			innerAngle(innerAngle),
			outerAngle(outerAngle) {}

		void SetDistance(const float& newDistance) { distance = newDistance; }
		void SetInnerAngle(const float& newInnerAngle) { innerAngle = newInnerAngle; }
		void SetOuterAngle(const float& newOuterAngle) { outerAngle = newOuterAngle; }

		const float& GetDistance() const { return distance; }
		const float& GetInnerAngle() const { return innerAngle; }
		const float& GetOuterAngle() const { return outerAngle; }

	protected:
		int GetVertexCount() const override { return 32; }

	private:
		float distance;
		float innerAngle;
		float outerAngle;
	};

	class DirectionalLightComponent : public LightComponent
	{
	public:
		DirectionalLightComponent(
			const LightConfig& config) :
			LightComponent(config) {}

	protected:
		int GetVertexCount() const override { return 32; }
	};

	class GameObject : public enable_shared_from_this<GameObject>
	{
	public:
		static inline unsigned int nextID;

		static shared_ptr<GameObject> Create(
			const string& name = "",
			unsigned int id = 0,
			bool isEnabled = true,
			const vec3& position = vec3(0),
			const vec3& rotation = vec3(0),
			const vec3& scale = vec3(1))
		{
			if (id == 0) id = ++nextID;
			string finalName = name.empty() 
				? "GameObject_" + to_string(id)
				: name;

			return make_shared<GameObject>(
				finalName, 
				id, 
				isEnabled, 
				position,
				rotation,
				scale);
		}

		GameObject(
			const string& name,
			const unsigned int& ID,
			const bool& isEnabled = true,
			const vec3& position = vec3(0),
			const vec3& rotation = vec3(0),
			const vec3& scale = vec3(1)) :
			name(name), 
			ID(ID), 
			isEnabled(isEnabled), 
			transform(make_shared<Transform>(position, rotation, scale)) {}

		virtual ~GameObject() = default;

		template <typename T, typename... Args>
		shared_ptr<T> AddComponent(Args&&... args)
		{
			auto component = make_shared<T>(std::forward<Args>(args)...);
			components[typeid(T).hash_code()] = component;
			return component;
		}

		template <typename T>
		shared_ptr<T> GetComponent()
		{
			auto it = components.find(typeid(T).hash_code());
			if (it != components.end())
			{
				return dynamic_pointer_cast<T>(it->second);
			}
			return nullptr;
		}

		void Update(float deltaTime)
		{
			for (auto& [_, component] : components)
			{
				component->Update(deltaTime);
			}
		}

		void Render(const mat4& view, const mat4& projection)
		{
			for (auto& [_, component] : components)
			{
				component->Render(view, projection);
			}
		}

		void SetName(const string& newName) { name = newName; }
		void SetID(const unsigned int& newID) { ID = newID; }
		void SetEnableState(const bool& newEnableState) { isEnabled = newEnableState; }

		void SetTxtFilePath(const string& newTxtFile) { txtFile = newTxtFile; }

		void AddChild(const shared_ptr<GameObject>& target, const shared_ptr<GameObject>& addedChild)
		{
			children.push_back(addedChild);
			addedChild->SetParent(target);
		}
		void RemoveChild(const shared_ptr<GameObject>& removedChild)
		{
			children.erase(
				std::remove(children.begin(), 
					children.end(), 
					removedChild), 
					children.end());
			removedChild->RemoveParent(removedChild);
		}

		const string& GetName() const { return name; }
		const unsigned int& GetID() const { return ID; }
		const bool& IsEnabled() const { return isEnabled; }
		const string& GetTxtFile() const { return txtFile; }
		const shared_ptr<Transform>& GetTransform() const { return transform; }
		const shared_ptr<GameObject>& GetParent() const { return parent; }
		const vector<shared_ptr<GameObject>>& GetChildren() const { return children; }

		void SetParent(const shared_ptr<GameObject>& newParent) { parent = newParent; }
		void RemoveParent(const shared_ptr<GameObject>&) { parent = nullptr; }

	private:
		string name;
		unsigned int ID;
		bool isEnabled;

		string txtFile;

		shared_ptr<Transform> transform; //non-removable core component
		shared_ptr<GameObject> parent;
		vector<shared_ptr<GameObject>> children;
		map<size_t, shared_ptr<Component>> components;
	};

	class GameObjectManager
	{
	public:
		static void RenderAll(const mat4& view, const mat4& projection);

		static inline bool renderBillboards = true;
		static inline bool renderLightBorders = true;

		static void AddGameObject(const shared_ptr<GameObject>& obj)
		{
			objects.push_back(obj);
		}
		static void AddBillboard(const shared_ptr<GameObject>& obj)
		{
			billboards.push_back(obj);
		}

		static void AddPointlight(const shared_ptr<GameObject>& obj)
		{
			pointLights.push_back(obj);
		}
		static void AddSpotLight(const shared_ptr<GameObject>& obj)
		{
			spotLights.push_back(obj);
		}
		static void SetDirectionalLight(const shared_ptr<GameObject>& newDirectionalLight)
		{
			directionalLight = newDirectionalLight;
		}

		static void SetSkybox(const shared_ptr<GameObject>& obj)
		{
			skybox = obj;
		}
		static void SetActionTex(const shared_ptr<GameObject>& obj)
		{
			selectedAction = obj;
		}
		static void SetBorder(const shared_ptr<GameObject>& obj)
		{
			border = obj;
		}

		static void DestroyGameObject(const shared_ptr<GameObject>& obj, bool localOnly)
		{
			objects.erase(std::remove(
				objects.begin(), 
				objects.end(), 
				obj), 
				objects.end());
		}

		static void FindAndDestroyGameObject(const string& objTxtFile, bool localOnly);

		static const vector<shared_ptr<GameObject>>& GetObjects()
		{
			return objects;
		}
		static const vector<shared_ptr<GameObject>>& GetBillboards()
		{
			return billboards;
		}

		static const vector<shared_ptr<GameObject>>& GetPointlights()
		{
			return pointLights;
		}
		static const vector<shared_ptr<GameObject>>& GetSpotLights()
		{
			return spotLights;
		}
		static const shared_ptr<GameObject> GetDirectionalLight()
		{
			return directionalLight;
		}

		static const shared_ptr<GameObject> GetSkybox()
		{
			return skybox;
		}
		static const shared_ptr<GameObject> GetActionTex()
		{
			return selectedAction;
		}
		static const shared_ptr<GameObject> GetSelectedObjectBorder()
		{
			return border;
		}

	private:
		static inline vector<shared_ptr<GameObject>> objects;			  //main list of all game objects
		static inline vector<shared_ptr<GameObject>> opaqueObjects;
		static inline vector<shared_ptr<GameObject>> transparentObjects;
		static inline vector<shared_ptr<GameObject>> billboards;
		static inline vector<shared_ptr<GameObject>> pointLights;
		static inline vector<shared_ptr<GameObject>> spotLights;
		static inline shared_ptr<GameObject> directionalLight;			  //single directional light
		static inline shared_ptr<GameObject> skybox;					  //single skybox
		static inline shared_ptr<GameObject> selectedAction;			  //single selected object action
		static inline shared_ptr<GameObject> border;					  //single selected object border
	};
}