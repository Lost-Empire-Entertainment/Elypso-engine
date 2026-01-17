//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/import_kfd.hpp"

#include "core/ku_registry.hpp"

namespace KalaUI::Core
{
	using std::string;
	using std::vector;

	using KalaHeaders::KalaFontData::GlyphHeader;
	using KalaHeaders::KalaFontData::GlyphTable;
	using KalaHeaders::KalaFontData::GlyphBlock;

	class LIB_API Font
	{
	public:
		static KalaUIRegistry<Font>& GetRegistry();

		//Loads a font from disk
		static Font* Initialize(
			const string& name,
			const string& fontPath);

		bool IsInitialized() const;

		u32 GetID() const;

		void SetName(const string& newName);
		const string& GetName() const;

		const string& GetPath() const;
		
		const GlyphHeader& GetGlyphHeader() const;
		const vector<GlyphTable>& GetGlyphTables() const;
		const vector<GlyphBlock>& GetGlyphBlocks() const;

		//Do not destroy manually, erase from registry instead
		~Font();
	private:
		bool isInitialized{};

		string name{};
		string fontPath{};
		
		GlyphHeader header{};
		vector<GlyphTable> tables{};
		vector<GlyphBlock> blocks{};

		u32 ID{};
	};
}