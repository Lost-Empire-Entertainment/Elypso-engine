//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "directionallight.hpp"
#include "billboard.hpp"
#include "render.hpp"
#include "console.hpp"
#include "selectobject.hpp"
#include "meshcomponent.hpp"
#include "lighttcomponent.hpp"
#include "materialcomponent.hpp"
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using glm::translate;
using glm::quat;

using Graphics::Components::Mesh;
using MeshType = Graphics::Components::Mesh::MeshType;
using Graphics::Shape::Billboard;
using Graphics::Render;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using Core::Select;
using Graphics::Components::DirectionalLightComponent;
using Graphics::Components::LightComponent;
using Graphics::Components::Material;
#if ENGINE_MODE
using Graphics::GUI::GUISceneWindow;
#endif

namespace Graphics::Shape
{
	shared_ptr<GameObject> DirectionalLight::InitializeDirectionalLight(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& txtFilePath,
		const vec3& diffuse,
		const float& intensity,
		string& name,
		unsigned int& id,
		const bool& isEnabled,
		const bool& isMeshEnabled,
		
		unsigned int& billboardID,
		const bool& isBillboardEnabled)
	{
		auto obj = GameObject::Create(
			name, 
			id, 
			isEnabled);
		if (obj == nullptr) Engine::CreateErrorPopup("Failed to initialize directional light");

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

		auto lightConfig = LightComponent::LightConfig
		{
			diffuse,
			intensity,
			isMeshEnabled,
			(path(Engine::filesPath) / "icons" / "directionalLight.png").string(),
			32,
			isBillboardEnabled
		};

		auto dirLight = obj->AddComponent<DirectionalLightComponent>
		(
			lightConfig
		);

		dirLight->Initialize(obj, vertices, "directional_light", pos, rot, scale);

		string objName = obj->GetName();
		if (obj->GetTransform() == nullptr) Engine::CreateErrorPopup(("Failed to assign transform component to " + objName).c_str());
		if (obj->GetComponent<Mesh>() == nullptr) Engine::CreateErrorPopup(("Failed to assign mesh component to " + objName).c_str());
		if (obj->GetComponent<Material>() == nullptr) Engine::CreateErrorPopup(("Failed to assign material component to '" + objName).c_str());

		obj->SetTxtFilePath(txtFilePath);

		GameObjectManager::AddGameObject(obj);
		GameObjectManager::AddOpaqueObject(obj);
		GameObjectManager::SetDirectionalLight(obj);

#if ENGINE_MODE
		GUISceneWindow::UpdateCounts();
#endif
		Select::selectedObj = obj;
		Select::isObjectSelected = true;

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Successfully initialized " + obj->GetName() + " with ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}
}