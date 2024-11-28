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

//external
#include "glad.h"
#include "magic_enum.hpp"
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "selectobject.hpp"
#include "shader.hpp"
#include "billboard.hpp"

namespace Core
{
	class Select;
}

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

	using Graphics::Shader;
	using Core::Select;

	class Component
	{
	public:
		virtual ~Component() = default;

		virtual void Initialize(const shared_ptr<GameObject>& parent)
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
			scale(scale)
		{
		}

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
			indices(indices)
		{
		}
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
			const MeshType& type,
			const GLuint& VAO,
			const GLuint& VBO,
			const GLuint& EBO) :
			isEnabled(isEnabled),
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

		void Render(const mat4& view, const mat4& projection) override
		{
			if (isEnabled)
			{
				
			}
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
			isBillboardEnabled(config.isBillboardEnabled) {
		}

		void Initialize(const shared_ptr<GameObject>& parent) override
		{
			this->parent = parent;

			// Mesh Initialization
			SetupMesh(parent);

			// Material Initialization
			auto material = parent->AddComponent<Material>();
			Shader shader = Shader::LoadShader(vertShader, fragShader);
			material->AddShader(vertShader, fragShader, shader);

			// Billboard Initialization
			SetupBillboard(parent);
		}

		void SetDiffuse(const vec3& newDiffuse) { diffuse = newDiffuse; }
		void SetIntensity(const float& newIntensity) { intensity = newIntensity; }

		const vec3& GetDiffuse() const { return diffuse; }
		const float& GetIntensity() const { return intensity; }

	protected:
		// Helper method for setting up shaders for rendering
		void SetupShaderUniforms(Shader& shader, const mat4& view, const mat4& projection, const vec3& lightColor)
		{
			shader.Use();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);
			shader.SetVec3("color", lightColor);

			float transparency = (Select::selectedObj == parent.lock() && Select::isObjectSelected) ? 1.0f : 0.5f;
			shader.SetFloat("transparency", transparency);
		}

		// Helper method for light border rendering (if common behavior is needed)
		void RenderLightBorders(const shared_ptr<Transform>& transform, Shader& shader, GLuint vao, int vertexCount)
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

		virtual void SetupMesh(const shared_ptr<GameObject>& parent) = 0;

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

				// Use the helper method to setup common shader uniforms
				SetupShaderUniforms(
					shader, 
					view, 
					projection, 
					GetDiffuse());

				// Render light borders (specific to PointLightComponent)
				if (GameObjectManager::renderLightBorders 
					&& mesh->IsEnabled())
				{
					RenderLightBorders(
						transform, 
						shader, 
						mesh->GetVAO(), 
						24);
				}
			}
		}

		void SetDistance(const float& newDistance) { distance = newDistance; }
		const float& GetDistance() const { return distance; }

	private:
		void SetupMesh(const shared_ptr<GameObject>& parent)
		{
			float vertices[] = 
			{
				-0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,
				0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f,
				0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
				-0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
				-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
				0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
				0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
				-0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
				-0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,
				0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f,
				0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
				-0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f,
			};

			GLuint vao, vbo;
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);

			parent->AddComponent<Mesh>(isMeshEnabled, Mesh::MeshType::point_light, vao, vbo, 0);
		}

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

				// Use the helper method to setup common shader uniforms
				SetupShaderUniforms(
					shader, 
					view, 
					projection, 
					GetDiffuse());

				// Render light borders (specific to SpotLightComponent)
				if (GameObjectManager::renderLightBorders 
					&& mesh->IsEnabled())
				{
					RenderLightBorders(
						transform, 
						shader, 
						mesh->GetVAO(), 
						32);
				}
			}
		}

		void SetDistance(const float& newDistance) { distance = newDistance; }
		void SetInnerAngle(const float& newInnerAngle) { innerAngle = newInnerAngle; }
		void SetOuterAngle(const float& newOuterAngle) { outerAngle = newOuterAngle; }

		const float& GetDistance() const { return distance; }
		const float& GetInnerAngle() const { return innerAngle; }
		const float& GetOuterAngle() const { return outerAngle; }

	private:
		void SetupMesh(const shared_ptr<GameObject>& parent)
		{
			float vertices[] = 
			{
				//four corner edges
				0.0f,  0.5f,  0.0f,
			   -0.5f, -0.5f, -0.5f,

				0.0f,  0.5f,  0.0f,
				0.5f, -0.5f, -0.5f,

				0.0f,  0.5f,  0.0f,
			   -0.5f, -0.5f,  0.5f,

				0.0f,  0.5f,  0.0f,
				0.5f, -0.5f,  0.5f,

				//four bottom edges
				0.5f, -0.5f,  0.5f,
			   -0.5f, -0.5f,  0.5f,

				0.5f, -0.5f, -0.5f,
			   -0.5f, -0.5f, -0.5f,

			   -0.5f, -0.5f, -0.5f,
			   -0.5f, -0.5f,  0.5f,

				0.5f, -0.5f, -0.5f,
				0.5f, -0.5f,  0.5f
			};

			GLuint vao, vbo;
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);

			parent->AddComponent<Mesh>(isMeshEnabled, Mesh::MeshType::spot_light, vao, vbo, 0);
		}

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

				// Use the helper method to setup common shader uniforms
				SetupShaderUniforms(
					shader, 
					view, 
					projection, 
					GetDiffuse());

				// Render light borders (specific to DirectionalLightComponent)
				if (GameObjectManager::renderLightBorders 
					&& mesh->IsEnabled())
				{
					RenderLightBorders(
						transform, 
						shader, 
						mesh->GetVAO(), 
						32);
				}
			}
		}

	private:
		void SetupMesh(const shared_ptr<GameObject>& parent)
		{
			float vertices[] = 
			{
				//four corner edges
				0.0f,  0.5f,  0.0f,
			   -0.5f, -0.5f, -0.5f,

				0.0f,  0.5f,  0.0f,
				0.5f, -0.5f, -0.5f,

				0.0f,  0.5f,  0.0f,
			   -0.5f, -0.5f,  0.5f,

				0.0f,  0.5f,  0.0f,
				0.5f, -0.5f,  0.5f,

				//four bottom edges
				0.5f, -0.5f,  0.5f,
			   -0.5f, -0.5f,  0.5f,

				0.5f, -0.5f, -0.5f,
			   -0.5f, -0.5f, -0.5f,

			   -0.5f, -0.5f, -0.5f,
			   -0.5f, -0.5f,  0.5f,

				0.5f, -0.5f, -0.5f,
				0.5f, -0.5f,  0.5f
			};

			GLuint vao, vbo;
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);

			parent->AddComponent<Mesh>(isMeshEnabled, Mesh::MeshType::directional_light, vao, vbo, 0);
		}
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
		static void SetDirectionalLight(const shared_ptr<GameObject>& newDirectionalLight)
		{
			directionalLight = newDirectionalLight;
		}
		static void SetSkybox(const shared_ptr<GameObject>& obj)
		{
			skybox = obj;
		}

		static void DestroyGameObject(const shared_ptr<GameObject>& obj)
		{
			objects.erase(std::remove(
				objects.begin(), 
				objects.end(), 
				obj), 
				objects.end());
		}

		static const vector<shared_ptr<GameObject>>& GetObjects()
		{
			return objects;
		}
		static const shared_ptr<GameObject> GetDirectionalLight()
		{
			return directionalLight;
		}
		static const shared_ptr<GameObject> GetSkybox()
		{
			return skybox;
		}

	private:
		static inline vector<shared_ptr<GameObject>> objects;    //main list of all game objects
		static inline shared_ptr<GameObject> directionalLight;   //single directional light
		static inline shared_ptr<GameObject> skybox;             //single skybox
	};
}