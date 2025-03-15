//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

//external
#include "quaternion.hpp"
#include "matrix_transform.hpp"

//engine
#include "audioobject.hpp"
#include "core.hpp"
#include "gameobject.hpp"
#include "selectobject.hpp"
#include "console.hpp"
#include "meshcomponent.hpp"
#include "billboard.hpp"
#include "audioplayercomponent.hpp"
#include "audio.hpp"
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
using Graphics::Components::AudioPlayerComponent;
using Core::Audio;
#if ENGINE_MODE
using Graphics::GUI::GUISceneWindow;
#endif

namespace Graphics::Shape
{
	shared_ptr<GameObject> AudioObject::InitializeAudioObject(
		const vec3& pos,
		const vec3& rot,
		const vec3& scale,
		const string& txtFilePath,
		string& name,
		unsigned int& id,
		const bool& isEnabled,

		const string& audioFileName,
		const bool& isPlaying,
		const bool& is3D,
		const float& volume,
		const float& minRange,
		const float& maxRange,

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
			//edges of the cube
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,

			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,

			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,

			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			//connecting edges
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,

			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,

			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f,  0.5f,
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
			MeshType::audio,
			vao,
			vbo,
			ebo);
		mesh->SetOwner(obj);

		string vert = (path(Engine::filesPath) / "shaders" / "Basic_model.vert").string();
		string frag = (path(Engine::filesPath) / "shaders" / "Basic.frag").string();

		if (!exists(vert)
			|| !exists(frag))
		{
			Engine::CreateErrorPopup("One of the shader paths for audio object is invalid!");
		}

		Shader audioObjectShader = Shader::LoadShader(vert, frag);

		auto mat = obj->AddComponent<MaterialComponent>();
		mat->SetOwner(obj);
		mat->AddShader(vert, frag, audioObjectShader);

		auto apc = obj->AddComponent<AudioPlayerComponent>(
			isPlaying,
			false,
			is3D,
			volume,
			minRange,
			maxRange);
		apc->SetOwner(obj);
		apc->SetName(audioFileName);

		string billboardDiffTexture = (path(Engine::filesPath) / "icons" / "audio.png").string();
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

		//import and play audio file after txt file path has been assigned
		if (audioFileName != "") Audio::Import(audioFileName, obj);
		if (isPlaying) Audio::Play(audioFileName, obj);

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
			"Successfully initialized audio object with name " + obj->GetName() + " and ID " + to_string(obj->GetID()) + "\n");

		return obj;
	}

	void AudioObject::RenderAudioObject(
		const shared_ptr<GameObject>& obj, 
		const mat4& view, 
		const mat4& projection)
	{
		if (!obj) Engine::CreateErrorPopup("Audio gameobject is invalid.");

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
				mat4 model = mat4(1.0f);
				model = translate(model, obj->GetComponent<TransformComponent>()->GetPosition());
				quat newRot = quat(radians(obj->GetComponent<TransformComponent>()->GetRotation()));
				model *= mat4_cast(newRot);
				model = scale(model, obj->GetComponent<TransformComponent>()->GetScale());

				shader.SetMat4("model", model);
				GLuint VAO = mesh->GetVAO();
				glBindVertexArray(VAO);
				glDrawArrays(GL_LINES, 0, 24);
			}
		}
	}
}