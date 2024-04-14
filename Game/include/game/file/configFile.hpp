//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace GameFile
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
			type_vec3
		};
		enum class ValueType
		{
			current_value,
			min_value,
			max_value
		};

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

		string GetName() const { return name; }

		string GetMinValue() const { return minValue; }
		string GetMaxValue() const { return maxValue; }
		string GetValue() const { return currentValue; };
		void SetValue(const string& newValue) { currentValue = newValue; }

		Type GetType() const { return type; };
	private:
		string name;
		string currentValue;
		string minValue;
		string maxValue;
		Type type;
	};

	class ConfigFileManager
	{
	public:
		static void SetDefaultConfigValues();

		static void LoadConfigFile();
		static void SaveConfigFile();
	private:
		static inline string configFilePath;
		static inline vector<ConfigFileValue> values;

		static void AddValue(const ConfigFileValue& value)
		{
			values.push_back(value);
		};

		static void UpdateValues();

		static bool IsValueInRange(
			const string& name,
			const string& value);

		static void CreateNewConfigFile();
	};
}