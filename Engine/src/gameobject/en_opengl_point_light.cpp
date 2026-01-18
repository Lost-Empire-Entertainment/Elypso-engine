//Copyright(C) 2026 Lost Empire Entertainment
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

#include "gameobject/en_opengl_point_light.hpp"
#include "core/en_core.hpp"
#include "graphics/en_opengl_functions.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::mat4;
using KalaHeaders::KalaMath::isnear;
using KalaHeaders::KalaMath::kclamp;
using KalaHeaders::KalaMath::addpos;
using KalaHeaders::KalaMath::setpos;
using KalaHeaders::KalaMath::getpos;
using KalaHeaders::KalaMath::addrot;
using KalaHeaders::KalaMath::setrot;
using KalaHeaders::KalaMath::getroteuler;
using KalaHeaders::KalaMath::getrotquat;
using KalaHeaders::KalaMath::addsize;
using KalaHeaders::KalaMath::setsize;
using KalaHeaders::KalaMath::getsize;
using KalaHeaders::KalaMath::getdirfront;
using KalaHeaders::KalaMath::getdirright;
using KalaHeaders::KalaMath::getdirup;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::OpenGL::OpenGL_Global;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::OpenGLFunctions::OpenGL_Functions_Engine;
using ElypsoEngine::Graphics::OpenGLFunctions::GL_Engine;

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
	static EngineRegistry<OpenGL_PointLight> registry{};

	EngineRegistry<OpenGL_PointLight>& OpenGL_PointLight::GetRegistry() { return registry; }

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
		
		u32 newID = EngineCore::GetGlobalID() + 1;
		EngineCore::SetGlobalID(newID);

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

	bool OpenGL_PointLight::IsInitialized() const { return isInitialized; }
	
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

		const GL_Engine* enFunc = OpenGL_Functions_Engine::GetGLEngine();

		if (isAlpha)
		{
			enFunc->glEnable(GL_BLEND);
			enFunc->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			enFunc->glDepthMask(GL_FALSE);
		}

		render.shader->SetVec3("uColor", render.color);
		render.shader->SetFloat("uOpacity", render.opacity);

		enFunc->glBindVertexArray(render.VAO);
		enFunc->glDrawElements(
			GL_TRIANGLES,
			render.indices.size(),
			GL_UNSIGNED_INT,
			0);
		enFunc->glBindVertexArray(0);

		if (isAlpha)
		{
			enFunc->glDisable(GL_BLEND);
			enFunc->glDepthMask(GL_TRUE);
		}

		return true;
	}

	u32 OpenGL_PointLight::GetID() const { return ID; }

	OpenGL_Context* OpenGL_PointLight::GetContext() const { return context; }

	void OpenGL_PointLight::SetName(const string& newName)
	{
		//skip if name is empty, same as existing or too long
		if (newName.empty()
			|| newName == name
			|| newName.length() > 50) return;

		name = newName;
	}
	const string& OpenGL_PointLight::GetName() { return name; }

	void OpenGL_PointLight::SetRenderDebugShapeState(bool newValue)
	{
		render.canUpdate = newValue;
		data.canRender = newValue;
	}
	bool OpenGL_PointLight::CanRenderDebugShape() const { return render.canUpdate; }

	void OpenGL_PointLight::SetRenderLightState(bool newValue)
	{
		data.canRender = newValue;
	}
	bool OpenGL_PointLight::CanRenderLight() const { return data.canRender; }

	const vector<vec3>& OpenGL_PointLight::GetVertices() const { return render.vertices; }
	const vector<u32>& OpenGL_PointLight::GetIndices() const { return render.indices; }

	vec3 OpenGL_PointLight::GetFront() { return getdirfront(transform); }
	vec3 OpenGL_PointLight::GetRight() { return getdirright(transform); }
	vec3 OpenGL_PointLight::GetUp() { return getdirup(transform); }

	void OpenGL_PointLight::AddPos(
		PosTarget type,
		const vec3& deltaPos)
	{
		addpos(
			transform,
			{},
			type,
			deltaPos);

		data.pos = getpos(
			transform,
			PosTarget::POS_COMBINED);
	}
	void OpenGL_PointLight::SetPos(
		PosTarget type,
		const vec3& newPos)
	{
		setpos(
			transform,
			{},
			type,
			newPos);

		data.pos = getpos(
			transform,
			PosTarget::POS_COMBINED);
	}
	vec3 OpenGL_PointLight::GetPos(PosTarget type)
	{
		return getpos(
			transform,
			type);
	}

	void OpenGL_PointLight::AddRot(
		RotTarget type,
		const vec3& deltaRot)
	{
		addrot(
			transform,
			{},
			type,
			deltaRot);
	}
	void OpenGL_PointLight::SetRot(
		RotTarget type,
		const vec3& newRot)
	{
		setrot(
			transform,
			{},
			type,
			newRot);
	}
	vec3 OpenGL_PointLight::GetRot(RotTarget type)
	{
		return getroteuler(
			transform,
			type);
	}
	quat OpenGL_PointLight::GetRotQuat(RotTarget type)
	{
		return getrotquat(
			transform,
			type);
	}

	void OpenGL_PointLight::AddSize(
		SizeTarget type,
		const vec3& deltaSize)
	{
		addsize(
			transform,
			{},
			type,
			deltaSize);
	}
	void OpenGL_PointLight::SetSize(
		SizeTarget type,
		const vec3& newSize)
	{
		setsize(
			transform,
			{},
			type,
			newSize);
	}
	vec3 OpenGL_PointLight::GetSize(SizeTarget type)
	{
		return getsize(
			transform,
			type);
	}

	void OpenGL_PointLight::SetNormalizedDebugColor(const vec3& newValue)
	{
		render.color = kclamp(newValue, 0.0f, 1.0f);
	}
	void OpenGL_PointLight::SetDebugRGBColor(const vec3& newValue)
	{
		int clampX = clamp(static_cast<int>(newValue.x), 0, 255);
		int clampY = clamp(static_cast<int>(newValue.y), 0, 255);
		int clampZ = clamp(static_cast<int>(newValue.z), 0, 255);

		f32 normalizedX = static_cast<f32>(clampX) / 255;
		f32 normalizedY = static_cast<f32>(clampY) / 255;
		f32 normalizedZ = static_cast<f32>(clampZ) / 255;

		render.color = vec3(normalizedX, normalizedY, normalizedZ);
	}

	const vec3& OpenGL_PointLight::GetNormalizedDebugColor() const { return render.color; }
	vec3 OpenGL_PointLight::GetDebugRGBColor() const
	{
		int rgbX = static_cast<int>(render.color.x * 255);
		int rgbY = static_cast<int>(render.color.y * 255);
		int rgbZ = static_cast<int>(render.color.z * 255);

		return vec3(rgbX, rgbY, rgbZ);
	}

	void OpenGL_PointLight::SetOpacity(f32 newValue)
	{
		f32 clamped = clamp(newValue, 0.0f, 1.0f);
		render.opacity = clamped;
	}
	f32 OpenGL_PointLight::GetOpacity() const { return render.opacity; }

	u32 OpenGL_PointLight::GetVAO() const { return render.VAO; }
	u32 OpenGL_PointLight::GetVBO() const { return render.VBO; }
	u32 OpenGL_PointLight::GetEBO() const { return render.EBO; }

	const OpenGL_Shader* OpenGL_PointLight::GetShader() const { return render.shader; }

	void OpenGL_PointLight::SetIntensity(f32 newValue)
	{
		f32 clamped = clamp(newValue, 0.0f, 5.0f);
		data.intensity = clamped;
	}
	f32 OpenGL_PointLight::GetIntensity() const { return data.intensity; }

	void OpenGL_PointLight::SetMaxRange(f32 newValue)
	{
		f32 clamped = clamp(newValue, 0.0f, 1000.0f);
		data.maxRange = clamped;
	}
	f32 OpenGL_PointLight::GetMaxRange() const { return data.maxRange; }

	void OpenGL_PointLight::SetNormalizedColor(const vec3& newValue)
	{
		data.color = kclamp(newValue, 0.0f, 1.0f);
	}
	void OpenGL_PointLight::SetRGBColor(const vec3& newValue)
	{
		int clampX = clamp(static_cast<int>(newValue.x), 0, 255);
		int clampY = clamp(static_cast<int>(newValue.y), 0, 255);
		int clampZ = clamp(static_cast<int>(newValue.z), 0, 255);

		f32 normalizedX = static_cast<f32>(clampX) / 255;
		f32 normalizedY = static_cast<f32>(clampY) / 255;
		f32 normalizedZ = static_cast<f32>(clampZ) / 255;

		data.color = vec3(normalizedX, normalizedY, normalizedZ);
	}

	vec3 OpenGL_PointLight::GetNormalizedColor() const { return data.color; }
	vec3 OpenGL_PointLight::GetRGBColor() const
	{
		int rgbX = static_cast<int>(data.color.x * 255);
		int rgbY = static_cast<int>(data.color.y * 255);
		int rgbZ = static_cast<int>(data.color.z * 255);

		return vec3(rgbX, rgbY, rgbZ);
	}

	void OpenGL_PointLight::SetConstant(f32 newValue)
	{
		f32 clamped = clamp(newValue, 0.0f, 5.0f);
		data.constant = clamped;
	}
	f32 OpenGL_PointLight::GetConstant() const { return data.constant; }

	void OpenGL_PointLight::SetLinear(f32 newValue)
	{
		f32 clamped = clamp(newValue, 0.0f, 1.0f);
		data.linear = clamped;
	}
	f32 OpenGL_PointLight::GetLinear() const { return data.linear; }

	void OpenGL_PointLight::SetQuadratic(f32 newValue)
	{
		f32 clamped = clamp(newValue, 0.0f, 1.0f);
		data.quadratic = clamped;
	}
	f32 OpenGL_PointLight::GetQuadratic() const { return data.quadratic; }

	const OpenGL_PointLight_Data* OpenGL_PointLight::GetDataPtr() const { return &data; }
	
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

		const GL_Engine* enFunc = OpenGL_Functions_Engine::GetGLEngine();

		if (render.VAO != 0)
		{
			enFunc->glDeleteVertexArrays(1, &render.VAO);
			render.VAO = 0;
		}
		if (render.VBO != 0)
		{
			enFunc->glDeleteBuffers(1, &render.VBO);
			render.VBO = 0;
		}
		if (render.EBO != 0)
		{
			enFunc->glDeleteBuffers(1, &render.EBO);
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
	
	const GL_Engine* enFunc = OpenGL_Functions_Engine::GetGLEngine();

	enFunc->glGenVertexArrays(1, &outVAO);
	enFunc->glGenBuffers(1, &outVBO);
	enFunc->glGenBuffers(1, &outEBO);
	
	enFunc->glBindVertexArray(outVAO);
	
	//VBO
	enFunc->glBindBuffer(GL_ARRAY_BUFFER, outVBO);
	enFunc->glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(vec3),
		vertices.data(),
		GL_STATIC_DRAW);
		
	//EBO
	enFunc->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outEBO);
	enFunc->glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(u32),
		indices.data(),
		GL_STATIC_DRAW);
		
	//position - layout 0
	enFunc->glEnableVertexAttribArray(0);
	enFunc->glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE,
		sizeof(vec3),
		(void*)0);
		
	enFunc->glBindVertexArray(0);
}