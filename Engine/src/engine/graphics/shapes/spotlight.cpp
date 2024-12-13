//Copyright(C) 2024 Lost Empire Entertainment
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
#if ENGINE_MODE
#include "gui_scenewindow.hpp"
#endif

using glm::translate;
using glm::quat;
using std::filesystem::exists;

using Graphics::Shape::Mesh;
using MeshType = Graphics::Shape::Mesh::MeshType;
using Graphics::Shape::Material;
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
		const vec3& diffuse,
		const float& intensity,
		const float& distance,
		const float& innerAngle,
		const float& outerAngle,
		string& name,
		unsigned int& id,
		const bool& isEnabled,
		const bool& isMeshEnabled,

		unsigned int& billboardID,
		const bool& isBillboardEnabled)
	{
		shared_ptr<Transform> transform = make_shared<Transform>(pos, rot, scale);

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

		shared_ptr<Mesh> mesh = make_shared<Mesh>(isMeshEnabled, MeshType::spot_light, vao, vbo, ebo);

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

		shared_ptr<Material> mat = make_shared<Material>();
		mat->AddShader(
			(path(Engine::filesPath) / "shaders" / "Basic_model.vert").string(),
			(path(Engine::filesPath) / "shaders" / "Basic.frag").string(), 
			spotlightShader);

		shared_ptr<SpotLight_Variables> spotLight =
			make_shared<SpotLight_Variables>(
				diffuse,
				intensity,
				distance,
				innerAngle,
				outerAngle);

		string billboardDiffTexture = (path(Engine::filesPath) / "icons" / "spotLight.png").string();
		shared_ptr<GameObject> billboard = Billboard::InitializeBillboard(
			pos,
			rot,
			scale,
			billboardDiffTexture,
			billboardID,
			isBillboardEnabled);

		shared_ptr<GameObject> obj = make_shared<GameObject>(
			true,
			name,
			id,
			isEnabled,
			transform,
			mesh,
			mat,
			spotLight);

		billboard->SetParentBillboardHolder(obj);
		obj->SetChildBillboard(billboard);

		obj->SetTxtFilePath(txtFilePath);

		GameObjectManager::AddGameObject(obj);
		GameObjectManager::AddOpaqueObject(obj);
		GameObjectManager::AddSpotLight(obj);

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

	void SpotLight::RenderSpotLight(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection)
	{
		if (obj == nullptr) Engine::CreateErrorPopup("Spotlight gameobject is invalid.");

		if (obj->IsEnabled())
		{
			Shader shader = obj->GetMaterial()->GetShader();

			shader.Use();
			shader.SetMat4("projection", projection);
			shader.SetMat4("view", view);

			float transparency = Select::selectedObj ==
				obj
				&& Select::isObjectSelected ? 1.0f : 0.5f;
			shader.SetFloat("transparency", transparency);
			shader.SetVec3("color", obj->GetSpotLight()->GetDiffuse());

			if (GameObjectManager::renderLightBorders
				&& obj->GetMesh()->IsEnabled())
			{
				mat4 model = mat4(1.0f);
				model = translate(model, obj->GetTransform()->GetPosition());
				quat newRot = quat(radians(obj->GetTransform()->GetRotation()));
				model *= mat4_cast(newRot);
				model = scale(model, obj->GetTransform()->GetScale());

				shader.SetMat4("model", model);
				GLuint VAO = obj->GetMesh()->GetVAO();
				glBindVertexArray(VAO);
				glDrawArrays(GL_LINES, 0, 32);
			}
		}
	}
}