//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

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

		//Constructor for config values that accept float, int, vec2, vec3, imvec2 and imvec4.
		ConfigFileValue(
			const string& name,
			const string& currentValue,
			const string& minValue,
			const string& maxValue,
			const Type& type) :
			name(name),
			currentValue(currentValue),
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
			values(values),
			type(type) {}

		string GetName() const { return name; }

		string GetMinValue() const { return minValue; }
		string GetMaxValue() const { return maxValue; }
		string GetValue() const { return currentValue; };
		void SetValue(const string& newValue) { currentValue = newValue; }
		string GetVectorValue(int index) const { return values[index]; }

		Type GetType() const { return type; };
	private:
		string name;
		string currentValue;
		string minValue;
		string maxValue;
		vector<string> values;
		Type type;
	};

	class ConfigFileManager
	{
	public:
		static void SetDefaultConfigValues();

		static void LoadConfigFile();
		static void SaveConfigFile();
	private:
		string configFilePath;
		vector<ConfigFileValue> values;

		void AddValue(const ConfigFileValue& value)
		{
			values.push_back(value);
		};

		void UpdateValues();

		bool IsValueInRange(
			const string& name,
			const string& value);

		void CreateNewConfigFile();
	};
}