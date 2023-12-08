#pragma once

#ifndef CORE_H
#define CORE_H

#include <string>

namespace Core
{
	extern const std::string name;
	extern const std::string version;

	/*
	* @brief Responsible for all the methods that are run when the engine is first initialized.
	*/
	void InitializeEngine();

	/*
	* @brief Responsible for all the methods that are run while the engine is actively running.
	*/
	void RunEngine();
}

#endif