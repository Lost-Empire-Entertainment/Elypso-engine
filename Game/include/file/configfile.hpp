//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>

using std::vector;
using std::string;

namespace GameFile
{
	class ConfigFile
	{
	public:
		/// <summary>
		/// Loads all config values into engine from confit txt file.
		/// </summary>
		static void LoadConfigFile();

		/// <summary>
		/// Saves all current config value map key values to config txt file.
		/// </summary>
		static void SaveConfigFile();

		/// <summary>
		/// Gets a value by key from the config value map.
		/// </summary>
		/// <param name="key"></param>
		/// <returns></returns>
		static string GetValue(const string& key);
		/// <summary>
		/// Assigns a value by key to config value map.
		/// </summary>
		/// <param name="key"></param>
		/// <param name="value"></param>
		static void SetValue(const string& key, const string& value);
	private:
		static inline string configFilePath;

		/// <summary>
		/// Temporary storage for all config settings until engine is saved.
		/// </summary>
		static inline vector<string> keys;
		static inline vector<string> values;

		/// <summary>
		/// Creates a brand new config file with default values.
		/// </summary>
		static void CreateNewConfigFile();
	};
}