//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <vector>
#include <memory>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/kw_core.hpp"
#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"

#include "gameobject/en_opengl_point_light.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::mat4;
using KalaHeaders::KalaMath::isnear;

using KalaWindow::Core::KalaWindowCore;
using namespace KalaWindow::OpenGL::OpenGLFunctions;
using KalaWindow::OpenGL::OpenGL_Global;

using std::string;
using std::to_string;
using std::vector;
using std::make_unique;
using std::unique_ptr;
using std::move;

static void CreateLightGeometry(
	const vector<vec3>& vertices,
	const vector<u32>& indices,
	u32& outVAO,
	u32& outVBO,
	u32& outEBO);

namespace ElypsoEngine::GameObject
{
	OpenGL_PointLight* OpenGL_PointLight::Initialize(
		const string& name,
		OpenGL_Context* context,
		vector<vec3> vertices,
		vector<u32> indices,
		OpenGL_Shader* shader)
	{
		if (!vertices.empty()
			&& !indices.empty())
		{
			if (!OpenGL_Global::IsContextValid(context))
			{
				Log::Print(
					"Cannot load point light '" + name + "' because the gl context is invalid!",
					"OPENGL_POINT_LIGHT",
					LogType::LOG_ERROR,
					2);

				return nullptr;
			}
			
			//shader is required if vertices and indices are passed
			if (!shader
				|| !shader->IsInitialized())
			{
				Log::Print(
					"Failed to load point light '" + name + "' because the shader context is invalid!",
					"OPENGL_POINT_LIGHT",
					LogType::LOG_ERROR,
					2);

				return {};
			}		
		}
		
		u32 newID = ++KalaWindowCore::globalID;
		unique_ptr<OpenGL_PointLight> newLight = make_unique<OpenGL_PointLight>();
		OpenGL_PointLight* lightPtr = newLight.get();
		
		Log::Print(
			"Loading point light '" + name + "' with ID '" + to_string(newID) + "'.",
			"OPENGL_POINT_LIGHT",
			LogType::LOG_DEBUG);
			
		if (!vertices.empty()
			&& !indices.empty())
		{
			lightPtr->render.shader = shader;
		
			lightPtr->render.vertices = move(vertices);
			lightPtr->render.indices = move(indices);
			
			CreateLightGeometry(
				lightPtr->render.vertices,
				lightPtr->render.indices,
				lightPtr->render.VAO,
				lightPtr->render.VBO,
				lightPtr->render.EBO);
		}
		
		lightPtr->ID = newID;
		lightPtr->context = context;
		lightPtr->SetName(name);
		
		lightPtr->isInitialized = true;
		
		registry.AddContent(newID, move(newLight));
		
		Log::Print(
			"Loaded point light '" + name + "' with ID '" + to_string(newID) + "'!",
			"OPENGL_POINT_LIGHT",
			LogType::LOG_SUCCESS);

		return lightPtr;
	}
	
	bool OpenGL_PointLight::Render(
		const mat4& view,
		const mat4& projection)
	{
		if (!render.canUpdate
			|| render.vertices.empty()
			|| render.indices.empty())
		{
			return false;
		}
		
		if (!OpenGL_Global::IsContextValid(context))
		{
			Log::Print(
                "Failed to render point light '" + name + "' because its gl context is invalid!",
                "OPENGL_POINT_LIGHT",
                LogType::LOG_ERROR,
                2);

            return false;
		}
		
		if (!render.shader)
		{
			Log::Print(
				"Failed to render point light '" + name + "' because its shader is invalid!",
				"OPENGL_POINT_LIGHT",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (!render.shader->Bind())
		{
			Log::Print(
				"Failed to render point light '" + name + "' because its shader '" + render.shader->GetName() + "' failed to bind!",
				"OPENGL_POINT_LIGHT",
				LogType::LOG_ERROR,
				2);

			return false;
		}
		
		u32 programID = render.shader->GetProgramID();

		vec3 pos = GetPos(PosTarget::POS_COMBINED);
		quat rot = GetRotQuat(RotTarget::ROT_COMBINED);
		vec3 size = GetSize(SizeTarget::SIZE_COMBINED);
		
		mat4 model = createumodel(pos, rot, size);

		render.shader->SetMat4("uModel", model);
		render.shader->SetMat4("uView", view);
		render.shader->SetMat4("uProjection", projection);

		bool isAlpha = isnear(render.opacity, 1.0f);

		if (isAlpha)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
		}

		render.shader->SetVec3("uColor", render.color);
		render.shader->SetFloat("uOpacity", render.opacity);

		glBindVertexArray(render.VAO);
		glDrawElements(
			GL_TRIANGLES,
			render.indices.size(),
			GL_UNSIGNED_INT,
			0);
		glBindVertexArray(0);

		if (isAlpha)
		{
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}

		return true;
	}
	
	OpenGL_PointLight::~OpenGL_PointLight()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot destroy point light '" + name + "' with ID '" + to_string(ID) + "' because it is not initialized!",
				"OPENGL_POINT_LIGHT",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Log::Print(
			"Destroying point light '" + name + "' with ID '" + to_string(ID) + "'.",
			"OPENGL_POINT_LIGHT",
			LogType::LOG_INFO);

		if (render.VAO != 0)
		{
			glDeleteVertexArrays(1, &render.VAO);
			render.VAO = 0;
		}
		if (render.VBO != 0)
		{
			glDeleteBuffers(1, &render.VBO);
			render.VBO = 0;
		}
		if (render.EBO != 0)
		{
			glDeleteBuffers(1, &render.EBO);
			render.EBO = 0;
		}
	}
}

void CreateLightGeometry(
	const vector<vec3>& vertices,
	const vector<u32>& indices,
	u32& outVAO,
	u32& outVBO,
	u32& outEBO)
{
	if (vertices.empty()
		|| indices.empty())
	{
		KalaWindowCore::ForceClose(
			"OpenGL point light error",
			"Failed to create point light geometry because vertices or indices were empty");
	}
	
	glGenVertexArrays(1, &outVAO);
	glGenBuffers(1, &outVBO);
	glGenBuffers(1, &outEBO);
	
	glBindVertexArray(outVAO);
	
	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, outVBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(vec3),
		vertices.data(),
		GL_STATIC_DRAW);
		
	//EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outEBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(u32),
		indices.data(),
		GL_STATIC_DRAW);
		
	//position - layout 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE,
		sizeof(vec3),
		(void*)0);
		
	glBindVertexArray(0);
}