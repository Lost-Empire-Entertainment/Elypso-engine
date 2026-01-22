//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/ku_registry.hpp"

constexpr u8 MAX_GLYPH_SIZE = 64U;

namespace KalaUI::Core
{
	using std::string;
	using std::vector;
	using std::array;
	using std::unordered_map;

	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaMath::Transform2D;

	struct Letter
	{
		u32 charCode{}; //glyph character code in unicode

		i16 bearingX{}; //glyph left bearing
		i16 bearingY{}; //glyph top bearing
		u16 advance{};  //glyph advance

		u32 texLayer = UINT32_MAX;

		Transform2D transform{};
	};

	class LIB_API Font
	{
	public:
		static KalaUIRegistry<Font>& GetRegistry();

		//Loads a font from disk
		static Font* Initialize(
			const string& name,
			const string& fontPath,
			uintptr_t glContext);

		bool IsInitialized() const;

		u32 GetID() const;

		void SetName(const string& newName);
		const string& GetName() const;

		const string& GetPath() const;
		
		const vector<Letter>& GetLetters() const;
		const Letter& GetLetter(u32 charCode) const;

		u32 GetTextureArrayID() const;
		u32 GetLayerCount() const;

		i16 GetAscent() const;
		i16 GetDescent() const;
		i16 GetLineGap() const;
		i16 GetLineAdvance() const;

		//Do not destroy manually, erase from registry instead
		~Font();
	private:
		u32 CreateLetterTextureArray(
			vector<Letter>& letters,
			const unordered_map<u32, vector<u8>>& rawPixels) const;

		bool isInitialized{};

		string name{};
		string fontPath{};
		u32 ID{};

		vector<Letter> letters{};

		u32 textureArrayID{};
		u32 layerCount{};

		i16 ascent{};
		i16 descent{};
		i16 lineGap{};
		i16 lineAdvance{};
	};
}