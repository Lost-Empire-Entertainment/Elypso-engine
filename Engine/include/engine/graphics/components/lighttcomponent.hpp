//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>

//engine
#include "gameobject.hpp"
#include "transformcomponent.hpp"

namespace Graphics::Components
{
	using std::shared_ptr;
	using std::weak_ptr;

	using Graphics::Shape::GameObject;
	using Graphics::Shape::Component;
	using Graphics::Components::Transform;

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

		void Initialize(
			const shared_ptr<GameObject>& parent, 
			const float* vertices,
			const vec3& pos,
			const vec3& rot,
			const vec3& scale) override;
		void Render(const mat4& view, const mat4& projection) override;

		void SetDiffuse(const vec3& newDiffuse) { diffuse = newDiffuse; }
		void SetIntensity(const float& newIntensity) { intensity = newIntensity; }

		const vec3& GetDiffuse() const { return diffuse; }
		const float& GetIntensity() const { return intensity; }

	protected:
		virtual int GetVertexCount() const = 0;

		void CastLight(Shader& shader, const mat4& view, const mat4& projection, const vec3& lightColor);
		void RenderBorder(const shared_ptr<Transform>& transform, Shader& shader, GLuint vao, int vertexCount);

		void SetupBillboard(const shared_ptr<GameObject>& parent) const;

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

		void SetDistance(const float& newDistance) { distance = newDistance; }
		const float& GetDistance() const { return distance; }

	protected:
		int GetVertexCount() const override { return 24; }

	private:

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

		void SetDistance(const float& newDistance) { distance = newDistance; }
		void SetInnerAngle(const float& newInnerAngle) { innerAngle = newInnerAngle; }
		void SetOuterAngle(const float& newOuterAngle) { outerAngle = newOuterAngle; }

		const float& GetDistance() const { return distance; }
		const float& GetInnerAngle() const { return innerAngle; }
		const float& GetOuterAngle() const { return outerAngle; }

	protected:
		int GetVertexCount() const override { return 32; }

	private:
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

	protected:
		int GetVertexCount() const override { return 32; }
	};
}