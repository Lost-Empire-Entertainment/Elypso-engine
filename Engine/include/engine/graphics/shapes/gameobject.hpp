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
#include "shader.hpp"
#include "billboard.hpp"
#include "core.hpp"

namespace Graphics::Components
{
	class Transform;
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
	using std::to_string;
	using std::filesystem::path;
	using std::forward;

	using Graphics::Shader;
	using Core::Engine;

	class AssimpVertex;

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
}

#include "transformcomponent.hpp"

namespace Graphics::Shape
{
	using Graphics::Components::Transform;

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
			auto component = make_shared<T>(forward<Args>(args)...);
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

		static void DestroyGameObject(const shared_ptr<GameObject>& obj, bool localOnly);
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