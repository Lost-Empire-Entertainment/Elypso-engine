//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

namespace EngineFile
{
	class ConfigFileValue
	{
	public:
		enum class Type
		{
			type_string,
			type_float,
			type_int,
			type_vec2,
			type_vec3,
			type_imvec2,
			type_imvec4
		};
		enum class ValueType
		{
			current_value,
			min_value,
			max_value
		};

		ConfigFileValue() :
			name(""),
			currentValue(""),
			defaultValue(""),
			minValue(""),
			maxValue(""),
			type(Type::type_string) {}

		//Constructor for config values that accept float, int, vec2, vec3, imvec2 and imvec4.
		ConfigFileValue(
			const string& name,
			const string& currentValue,
			const string& minValue,
			const string& maxValue,
			const Type& type) :
			name(name),
			currentValue(currentValue),
			defaultValue(currentValue),
			minValue(minValue),
			maxValue(maxValue),
			type(type) {}

		//Constructor for config values that accept an imgui direction vector.
		ConfigFileValue(
			const string& name,
			const string& currentValue,
			const vector<string> values,
			const Type& type) :
			name(name),
			currentValue(currentValue),
			defaultValue(currentValue),
			values(values),
			type(type) {}

		string GetName() const { return name; }

		string GetValue() const { return currentValue; }
		string GetDefaultValue() const { return defaultValue; }
		string GetMinValue() const { return minValue; }
		string GetMaxValue() const { return maxValue; }

		void SetValue(const string& newValue) { currentValue = newValue; }
		string GetVectorValue(int index) const { return values[index]; }

		Type GetType() const { return type; };
	private:
		string name;
		string currentValue;
		string defaultValue;
		string minValue;
		string maxValue;
		vector<string> values;
		Type type;
	};

	class ConfigFileManager
	{
	public:
		static inline vector<string> imgui_dir =
		{
			"ImGuiDir_Up",
			"ImGuiDir_Down",
			"ImGuiDir_Left",
			"ImGuiDir_Right",
			"ImGuiDir_None"
		};

		static void LoadConfigFile();
		static void SaveConfigFile();

		static inline vector<ConfigFileValue> values;
		static inline map<string, ConfigFileValue> valuesMap;
	private:
		static inline string configFilePath;
		
		static void AddValue(const ConfigFileValue& value)
		{
			values.push_back(value);
		};

		/// <param name="reset">Optional bool state to reset all values to default</param>
		static void UpdateValues(bool reset = false);
		static void ResetConfigValues();
		static void ResetGUIValues();
		static void ResetGUIColorValues();

		static bool IsValueInRange(
			const ConfigFileValue& type,
			const vector<string>& value);

		static void CreateNewConfigFile();
	};
}