#pragma once

#ifndef CORE_H
#define CORE_H

#include <string>

using namespace std;

namespace Core
{
	static const string name = "Elypso engine";
	static const string version = "0.0.4 Prototype";

	void InitializeEngine();
	void RunEngine();
}

#endif