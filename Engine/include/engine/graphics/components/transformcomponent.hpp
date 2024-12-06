//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//external
#include "glm.hpp"

//engine
#include "gameobject.hpp"

namespace Graphics::Components
{
	using glm::vec3;

	using Graphics::Shape::Component;

	class Transform : public Component
	{
	public:
		Transform(
			const vec3& position,
			const vec3& rotation,
			const vec3& scale) :
			position(position),
			rotation(rotation),
			scale(scale) {}

		void Update(float deltaTime) override {}

		void SetPosition(const vec3& newPosition)
		{
			position = newPosition;
		}
		void SetRotation(const vec3& newRotation)
		{
			rotation = newRotation;
		}
		void SetScale(const vec3& newScale)
		{
			scale = newScale;
		}

		const vec3& GetPosition() const
		{
			return position;
		}
		const vec3& GetRotation() const
		{
			return rotation;
		}
		const vec3& GetScale() const
		{
			return scale;
		}
	private:
		vec3 position;
		vec3 rotation;
		vec3 scale;
	};
}