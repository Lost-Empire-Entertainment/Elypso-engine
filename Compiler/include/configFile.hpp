//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

namespace Core
{
	using std::string;
	using std::vector;

	class ConfigFile
	{
	public:
		static void SaveData();
		static void LoadData();

		static void CreateNewConfigFile();

		static string GetValue(const string& key);
		static void SetValue(const string& key, const string& value);
	private:
		static inline vector<string> keys;
		static inline vector<string> values;
	};
}