//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>
#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/import_kmd.hpp"

#include "utils/kg_registry.hpp"

namespace KalaGraphics::Graphics
{
	using std::vector;
	using std::string;
	
	using KalaHeaders::ModelTable;
	using KalaHeaders::ModelBlock;
	using KalaHeaders::vec3;
	using KalaHeaders::quat;
	using KalaHeaders::toquat;
	
	using KalaGraphics::Utils::KalaGraphicsRegistry;
	
	class LIB_API Model
	{
	public:
		static inline KalaGraphicsRegistry<Model> registry{};
		
		//Initialize all models from a .kmd file. Returns a vector of non-owning pointers
		//because a .kmd file may hold more than one model
		static vector<Model*> InitializeAll(const string& modelPath);
		
		//Stream models based off of the provided tables
		static vector<Model*> StreamModels(
			const string& modelPath,
			const vector<ModelTable>& modelTables);
		
		inline bool IsInitialized() const { return isInitialized; }

		inline u32 GetID() const { return ID; }

		inline void SetName(const string& newName)
		{
			//skip if name is empty
			if (newName.empty()) return;
			//skip if name is too long
			if (newName.length() > 50) return;
			//skip if name is already same
			if (newName == name) return;

			name = newName;
		}
		inline const string& GetName() { return name; }
		
		inline vec3 GetBakedPos()  { return modelData.position; }
		inline quat GetBakedRot()  { return toquat(modelData.rotation); }
		inline vec3 GetBakedSize() { return modelData.size; }
		
		~Model() {};
	private:
		bool isInitialized{};

		string name{};
		
		ModelBlock modelData{};

		u32 ID{};
	};
}