//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

//external
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "spotlight.hpp"
#include "render.hpp"
#include "selectobject.hpp"
#include "billboard.hpp"
#include "console.hpp"
#include "transformcomponent.hpp"
#include "meshcomponent.hpp"
#include "materialcomponent.hpp"
#include "lightcomponent.hpp"
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using glm::translate;
using glm::quat;
using std::filesystem::exists;
using std::to_string;

using Graphics::Components::TransformComponent;
using Graphics::Components::MeshComponent;
using Graphics::Components::MaterialComponent;
using Graphics::Components::LightComponent;
using MeshType = Graphics::Components::MeshComponent::MeshType;
using Graphics::Render;
using Core::Select;
using Core::ConsoleManager;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;
#if ENGINE_MODE
using Graphics::GUI::GUISceneWindow;
#endif

namespace Graphics::Shape
{
	shared_ptr<GameObject> SpotLight::InitializeSpotLight(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& txtFilePath,
		const bool& canCastShadows,
		const vec3& diffuse,
		const float& intensity,
		const float& farPlane,
		const float& nearPlane,
		const float& innerAngle,
		const float& outerAngle,
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
			MeshType::spot_light, 
			vao, 
			vbo, 
			ebo);
		mesh->SetOwner(obj);

		string vert = (path(Engine::filesPath) / "shaders" / "Basic_model.vert").string();
		string frag = (path(Engine::filesPath) / "shaders" / "Basic.frag").string();

		if (!exists(vert)
			|| !exists(frag))
		{
			Engine::CreateErrorPopup("One of the shader paths for spotlight is invalid!");
		}

		Shader spotlightShader = Shader::LoadShader(
			(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
			(path(Engine::filesPath) / "shaders" / "Basic.frag").string());

		auto mat = obj->AddComponent<MaterialComponent>();
		mat->SetOwner(obj);
		mat->AddShader(
			(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
			(path(Engine::filesPath) / "shaders" / "Basic.frag").string(), 
			"",
			spotlightShader);

		auto spotlight = obj->AddComponent<LightComponent>(
			LightComponent::LightType::Spot,
			canCastShadows,
			diffuse,
			intensity,
			farPlane,
			nearPlane,
			innerAngle,
			outerAngle);
		spotlight->SetOwner(obj);

		string billboardDiffTexture = (path(Engine::filesPath) / "icons" / "spotLight.png").string();
		shared_ptr<GameObject> billboard = Billboard::InitializeBillboard(
			false,
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
		GameObjectManager::AddSpotLight(obj);

		//
		// SPOT SHADOW START
		//

		const auto& spotLights = GameObjectManager::GetSpotLights();

		unsigned int fbo{};
		unsigned int texture{};

		glGenFramebuffers(1, &fbo);
		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_DEPTH_COMPONENT,
			Render::shadowWidth,
			Render::shadowHeight,
			0,
			GL_DEPTH_COMPONENT,
			GL_FLOAT,
			nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D,
			texture,
			0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Render::spotShadowFBOs[obj] = fbo;
		Render::spotShadowMaps[obj] = texture;

		//
		// SPOT SHADOW END
		//

#if ENGINE_MODE
		GUISceneWindow::UpdateCounts();
#endif
		Select::selectedObj = obj;
		Select::isObjectSelected = true;

		ConsoleManager::WriteConsoleMessage(
			Caller::FILE,
			Type::DEBUG,
			"Successfully initialized Spotlight with name " + obj->GetName() + " and ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}

	void SpotLight::RenderSpotLight(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection)
	{
		if (obj == nullptr) Engine::CreateErrorPopup("Spotlight gameobject is invalid.");

		if (obj->IsEnabled())
		{
			auto mat = obj->GetComponent<MaterialComponent>();
			
			Shader shader = mat->GetShader();

			shader.Use();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);

			float transparency = Select::selectedObj ==
				obj
				&& Select::isObjectSelected ? 1.0f : 0.5f;
			shader.SetFloat("transparency", transparency);

			auto light = obj->GetComponent<LightComponent>();
			if (light)
			{
				shader.SetVec3("color", light->GetDiffuse());
			}

			auto mesh = obj->GetComponent<MeshComponent>();
			if (GameObjectManager::renderBorders)
			{
				mat4 model = mat4(1.0f);
				model = translate(model, obj->GetComponent<TransformComponent>()->GetPosition());
				quat newRot = quat(radians(obj->GetComponent<TransformComponent>()->GetRotation()));
				model *= mat4_cast(newRot);
				model = scale(model, obj->GetComponent<TransformComponent>()->GetScale());

				shader.SetMat4("model", model);
				GLuint VAO = mesh->GetVAO();
				glBindVertexArray(VAO);
				glDrawArrays(GL_LINES, 0, 32);
			}
		}
	}
}