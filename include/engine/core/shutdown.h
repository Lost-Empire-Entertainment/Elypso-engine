#pragma once

#ifndef CORE_SHUTDOWN_H
#define CORE_SHUTDOWN_H

//engine
#include "shader.h"

namespace Core
{
	class ShutdownManager
	{
	public:
		static void Shutdown();
	};
}

#endif