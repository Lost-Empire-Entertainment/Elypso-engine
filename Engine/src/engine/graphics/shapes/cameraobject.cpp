//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

//external
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "cameraobject.hpp"
#include "core.hpp"
#include "gameobject.hpp"
#include "selectobject.hpp"
#include "console.hpp"
#include "meshcomponent.hpp"
#include "billboard.hpp"
#include "cameracomponent.hpp"
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using glm::mat4_cast;
using glm::quat;
using std::filesystem::exists;
using std::filesystem::path;
using std::to_string;

using Core::Engine;
using Core::Select;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
using MeshType = Graphics::Components::MeshComponent::MeshType;
using Graphics::Components::CameraComponent;
#if ENGINE_MODE
using Graphics::GUI::GUISceneWindow;
#endif

namespace Graphics::Shape
{
	shared_ptr<GameObject> CameraObject::InitializeCameraObject(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& txtFilePath,
		string& name,
		unsigned int& id,
		const bool& isEnabled,

		unsigned int& billboardID,
		const bool& isBillboardEnabled)
	{
		auto obj = make_shared<GameObject>(name, id, isEnabled);
		auto transform = obj->AddComponent<TransformComponent>();
		transform->SetOwner(obj);
		transform->SetPosition(pos);
		transform->SetRotation(rot);
		transform->SetScale(scale);
		obj->SetEnableState(isEnabled);

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

		GLuint vao, vbo, ebo;

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		auto mesh = obj->AddComponent<MeshComponent>(
			MeshType::camera,
			vao,
			vbo,
			ebo);
		mesh->SetOwner(obj);

		string vert = (path(Engine::filesPath) / "shaders" / "Basic_model.vert").string();
		string frag = (path(Engine::filesPath) / "shaders" / "Basic.frag").string();

		if (!exists(vert)
			|| !exists(frag))
		{
			Engine::CreateErrorPopup("One of the shader paths for camera object is invalid!");
		}

		Shader cameraObjectShader = Shader::LoadShader(vert, frag);

		auto mat = obj->AddComponent<MaterialComponent>();
		mat->SetOwner(obj);
		mat->AddShader(vert, frag, cameraObjectShader);

		auto cc = obj->AddComponent<CameraComponent>();
		cc->SetOwner(obj);

		string billboardDiffTexture = (path(Engine::filesPath) / "icons" / "camera.png").string();
		auto billboard = Billboard::InitializeBillboard(
			pos,
			rot,
			scale,
			billboardDiffTexture,
			billboardID,
			isBillboardEnabled);

		billboard->SetParentBillboardHolder(obj);
		obj->SetChildBillboard(billboard);

		obj->SetTxtFilePath(txtFilePath);

		GameObjectManager::AddGameObject(obj);
		GameObjectManager::AddOpaqueObject(obj);

#if ENGINE_MODE
		GUISceneWindow::UpdateCounts();
#endif
		Select::selectedObj = obj;
		Select::isObjectSelected = true;

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Successfully initialized camera object with name " + obj->GetName() + " and ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}

	void CameraObject::RenderCameraObject(
		const shared_ptr<GameObject>& obj,
		const mat4& view,
		const mat4& projection)
	{
		if (!obj) Engine::CreateErrorPopup("Camera gameobject is invalid.");

		//dont render SceneCamera mesh
		//if (obj->GetName() == "SceneCamera") return;

		if (obj->IsEnabled())
		{
			auto mat = obj->GetComponent<MaterialComponent>();

			Shader shader = mat->GetShader();

			shader.Use();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);

			float transparency =
				Select::selectedObj == obj
				&& Select::isObjectSelected ? 1.0f : 0.5f;
			shader.SetFloat("transparency", transparency);
			shader.SetVec3("color", vec3(1.0f));

			auto mesh = obj->GetComponent<MeshComponent>();
			if (GameObjectManager::renderBorders)
			{
				auto cc = obj->GetComponent<CameraComponent>();
				auto tc = obj->GetComponent<TransformComponent>();

				mat4 model = mat4(1.0f);
				model = translate(model, tc->GetPosition());

				quat newRot = quat(radians(cc->GetFront()));
				model *= mat4_cast(newRot);

				model = scale(model, tc->GetScale());

				shader.SetMat4("model", model);
				GLuint VAO = mesh->GetVAO();
				glBindVertexArray(VAO);
				glDrawArrays(GL_LINES, 0, 32);
			}
		}
	}
}