//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

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
		static void ProcessFirstConfigValues();
		static void ProcessConfigFile(const string& fileName);
		static void SaveData();
	private:
		static inline vector<ConfigFileValue> values;

		static void SetConfigValuesToDefaultValues();

		static void AddValue(const ConfigFileValue& value) { values.push_back(value); };

		static string GetValue(const string& name);

		static void UpdateValues();

		static bool IsValueInRange(
			const string& name,
			const string& value);
	};
}