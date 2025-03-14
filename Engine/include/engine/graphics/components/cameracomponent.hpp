//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <typeindex>

//engine
#include "component.hpp"

namespace Graphics::Components
{
	using std::type_index;

	class CameraComponent : public Component
	{
	public:
        CameraComponent(
			float test1) : 
			test1(test1) {}

		void SetTest1(float newTest1) { test1 = newTest1; }

		float GetTest1() const { return test1; }

        type_index GetType() const override { return typeid(CameraComponent); }

        void Update(float deltaTime) override {}
	private:
		float test1;
	};
}