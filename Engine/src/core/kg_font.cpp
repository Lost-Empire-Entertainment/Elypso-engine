//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/file_utils.hpp"
#include "KalaHeaders/import_kfd.hpp"

#include "core/kg_font.hpp"
#include "core/kg_core.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaFontData::ResultToString;
using KalaHeaders::KalaFontData::ImportResult;
using KalaHeaders::KalaFontData::ImportKFD;
using KalaHeaders::KalaFontData::GlyphHeader;
using KalaHeaders::KalaFontData::GlyphTable;
using KalaHeaders::KalaFontData::GlyphBlock;

using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::is_regular_file;
using std::move;

namespace KalaGL::Core
{
	Font* Font::Initialize(
		const string& name,
		const string& fontPath)
	{
		u32 newID = ++KalaGLCore::globalID;
		unique_ptr<Font> newFont = make_unique<Font>();
		Font* fontPtr = newFont.get();

		Log::Print(
			"Loading font '" + name + "' with ID '" + to_string(newID) + "'.",
			"FONT",
			LogType::LOG_DEBUG);

		GlyphHeader header{};
		vector<GlyphTable> tables{};
		vector<GlyphBlock> blocks{};
		
		ImportResult result = ImportKFD(
			path(fontPath),
			header,
			tables,
			blocks);
			
		if (result != ImportResult::RESULT_SUCCESS)
		{
			Log::Print(
				"Failed to import font from path '" + fontPath + "'! Reason: " + ResultToString(result),
				"FONT",
				LogType::LOG_ERROR,
				2);
			
			return nullptr;
		}
		
		fontPtr->header = move(header);
		fontPtr->tables = move(tables);
		fontPtr->blocks = move(blocks);
		fontPtr->ID = newID;
		fontPtr->SetName(name);
		fontPtr->fontPath = fontPath;

		registry.AddContent(newID, move(newFont));

		Log::Print(
			"Loaded font '" + name + "' with ID '" + to_string(newID) + "'!",
			"FONT",
			LogType::LOG_SUCCESS);

		return fontPtr;
	}

	Font::~Font()
	{
		Log::Print(
			"Destroying font '" + name + "' with ID '" + to_string(ID) + "' .",
			"FONT",
			LogType::LOG_INFO);
	}
}