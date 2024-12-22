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
#include <typeindex>

//external
#include "glad.h"
#include "magic_enum.hpp"

//engine
#include "shader.hpp"

namespace Graphics::Components
{
	class Component;
	class TransformComponent;
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
	using std::string;
	using std::type_index;
	using std::enable_shared_from_this;

	using Graphics::Shader;
	using Graphics::Components::TransformComponent;
	using Graphics::Components::Component;

	class GameObject : public enable_shared_from_this<GameObject>
	{
	public:
		GameObject(const string& name, const string& txtFilePath)
			: name(name),
			ID(nextID++),
			isInitialized(false),
			isEnabled(true),
			txtFilePath(txtFilePath),
			transform(make_shared<TransformComponent>())
		{
			AddComponent<TransformComponent>(transform);
		}

		/*
		*
		* COMPONENTS
		*
		*/
		template <typename T, typename... Args>
		shared_ptr<T> AddComponent(Args&&... args)
		{
			auto component = make_shared<T>(forward<Args>(args)...);
			components[typeid(T)] = component;
			return component;
		}
		template <typename T>
		shared_ptr<T> GetComponent() const
		{
			auto it = components.find(typeid(T));
			if (it != components.end())
			{
				return static_pointer_cast<T>(it->second);
			}
			return nullptr;
		}
		template <typename T>
		void RemoveComponent()
		{
			components.erase(typeid(T));
		}

		/*
		* 
		* PARENT
		* 
		*/
		void SetParent(const shared_ptr<GameObject>& newParent)
		{
			parent = newParent;
		}
		shared_ptr<GameObject> GetParent() const
		{
			return parent.lock();
		}
		void RemoveParent()
		{
			if (auto currentParent = parent.lock())
			{
				currentParent->RemoveChild(shared_from_this());
			}
			parent.reset();
		}

		/*
		* 
		* CHILDREN
		* 
		*/
		void AddChild(const shared_ptr<GameObject>& child)
		{
			children.push_back(child);
			child->SetParent(shared_from_this());
		}
		const vector<shared_ptr<GameObject>>& GetChildren() const { return children; }
		void RemoveChild(const shared_ptr<GameObject>& child)
		{
			children.erase(remove(children.begin(), children.end(), child), children.end());
			if (child->GetParent().get() == this)
			{
				child->SetParent(nullptr);
			}
		}

		/*
		*
		* BASIC PROPERTIES SHARED ACROSS ALL GAMEOBJECTS
		*
		*/
		void SetName(const string& newName) { name = newName; }
		const string& GetName() const { return name; }

		void SetTxtFilePath(const string& newTxtFilePath) { txtFilePath = newTxtFilePath; }
		const string& GetTxtFilePath() const { return txtFilePath; }

		void SetEnableState(bool newEnableState) { isEnabled = newEnableState; }
		bool IsEnabled() const { return isEnabled; }

		void Initialize() { isInitialized = true; }
		bool IsInitialized() const { return isInitialized; }

		unsigned int GetID() const { return ID; }

		const shared_ptr<TransformComponent>& GetTransform() const { return transform; }
		void SetTransform(const shared_ptr<TransformComponent>& newTransform)
		{
			transform = newTransform;
			AddComponent<TransformComponent>(newTransform);
		}

		weak_ptr<GameObject> parent;
		vector<shared_ptr<GameObject>> children;

	private:
		static inline unsigned int nextID = 0;

		string name;
		unsigned int ID;
		bool isInitialized;
		bool isEnabled;
		string txtFilePath;

		shared_ptr<TransformComponent> transform;

		unordered_map<type_index, shared_ptr<Component>> components;
	};

	class GameObjectManager
	{
	public:
		static void RenderAll(
			const mat4& view,
			const mat4& projection);

		static inline bool renderBillboards = true;
		static inline bool renderLightBorders = true;

		static void SetCategoryNames(const map<string, vector<string>>& newCategoryNames)
		{
			categoryNames = newCategoryNames;
		}
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
		static void SetDirectionalLight(const shared_ptr<GameObject>& newDirectionalLight)
		{
			directionalLight = newDirectionalLight;
		}
		static void SetActionTex(const shared_ptr<GameObject>& newActionTex)
		{
			actionTex = newActionTex;
		}
		static void SetBorder(const shared_ptr<GameObject>& newBorder)
		{
			border = newBorder;
		}
		static void AddBillboard(const shared_ptr<GameObject>& obj)
		{
			billboards.push_back(obj);
		}
		static void SetSkybox(const shared_ptr<GameObject>& obj)
		{
			skybox = obj;
		}

		/// <summary>
		/// Used for deleting gameobject directly in scene.
		/// </summary>
		static void DestroyGameObject(const shared_ptr<GameObject>& obj, bool localOnly = true);
		/// <summary>
		/// Used for trying to find gameobject through its txt file and deleting it that way in scene.
		/// </summary>
		static void FindAndDestroyGameObject(const string& objTxtFile, bool localOnly = true);

		static const vector<shared_ptr<GameObject>>& GetObjects()
		{
			return objects;
		}
		static const vector<shared_ptr<GameObject>> GetPointLights()
		{
			return pointLights;
		}
		static const vector<shared_ptr<GameObject>> GetSpotLights()
		{
			return spotLights;
		}
		static const shared_ptr<GameObject> GetDirectionalLight()
		{
			return directionalLight;
		}
		static const shared_ptr<GameObject> GetActionTex()
		{
			return actionTex;
		}
		static const shared_ptr<GameObject> GetBorder()
		{
			return border;
		}
		static const vector<shared_ptr<GameObject>> GetBillboards()
		{
			return billboards;
		}
		static const shared_ptr<GameObject> GetSkybox()
		{
			return skybox;
		}
	private:
		static inline map<string, vector<string>> categoryNames;
		static inline vector<shared_ptr<GameObject>> objects;
		static inline vector<shared_ptr<GameObject>> opaqueObjects;
		static inline vector<shared_ptr<GameObject>> transparentObjects;
		static inline vector<shared_ptr<GameObject>> pointLights;
		static inline vector<shared_ptr<GameObject>> spotLights;
		static inline shared_ptr<GameObject> directionalLight;
		static inline shared_ptr<GameObject> actionTex;
		static inline shared_ptr<GameObject> border;
		static inline vector<shared_ptr<GameObject>> billboards;
		static inline shared_ptr<GameObject> skybox;
	};
}