//external
#include "glfw3.h"

//engine
#include "deltaTime.h"

namespace Core
{
	void DeltaTime::UpdateDeltaTime()
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltatime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}
}