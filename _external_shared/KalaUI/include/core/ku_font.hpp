//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <array>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/ku_registry.hpp"

namespace KalaUI::Core
{
	using std::string;
	using std::vector;
	using std::array;

	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaMath::Transform2D;

	struct Letter
	{
		u32 charCode{};         //glyph character code in unicode

		u16 width{};            //glyph width
		u16 height{};           //glyph height
		i16 bearingX{};         //glyph left bearing
		i16 bearingY{};         //glyph top bearing
		u16 advance{};          //glyph advance
		u32 rawPixelSize{};     //size of this glyph's pixels
		vector<u8> rawPixels{}; //8-bit raw pixels of this glyph (0 - 255, 0 is transparent, 255 is white)

		u32 VAO{};
		u32 VBO{};
		u32 EBO{};

		vector<vec2> vertices =
		{
			vec2(-0.5f,  0.5f), //top-left
			vec2(0.5f,  0.5f),  //top-right
			vec2(0.5f, -0.5f),  //bottom-right
			vec2(-0.5f, -0.5f)  //bottom-left
		};
		vector<u32> indices = 
		{
			0, 1, 2,
			2, 3, 0
		};
		vector<u32> uvs =
		{
			0, 0,
			1, 0,
			1, 1,
			0, 1
		};

		u32 glID{};

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

		//Do not destroy manually, erase from registry instead
		~Font();
	private:
		static void LoadLetterTexture(
			const vector<u8>& rawPixels,
			const array<array<i16, 2>, 4>& inVertices,
			const u16 width,
			const u16 height,
			Letter& letter);

		bool isInitialized{};

		string name{};
		string fontPath{};

		vector<Letter> letters{};

		u32 ID{};
	};
}