//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/core_utils.hpp"

#include "opengl/ui/ku_opengl_widget.hpp"
#include "core/ku_font.hpp"

namespace KalaUI::OpenGL
{
	class OpenGL_Manager; //forward-declare the manager
}

namespace KalaUI::OpenGL::UI
{
	using KalaUI::Core::Letter;

	class LIB_API OpenGL_Text : public OpenGL_Widget
	{
		friend class OpenGL_Manager; //friend-include the manager
	public:
		//Initialize a new Text widget.
		//Parent widget and texture are optional
		static OpenGL_Text* Initialize(
			const string& name,
			u32 windowID,
			uintptr_t glContext,
			const vector<u32>& charCodes,
			u32 fontID,
			const vec2 pos,
			const f32 rot,
			const f32 size,
			OpenGL_Shader* shader,
			OpenGL_Widget* parentWidget = {});
			
		virtual bool Render(
			const mat4& projection,
			f32 viewportHeight) override;

		void AddLetter(
			u32 newCharCode, 
			u32 slot = UINT32_MAX);
		void AddTab();
		void AddNewLine();
		void RemoveLetterFromSlot(u32 slot);
		
		void SetText(const vector<u32>& newValue);
		vector<u32>& GetText();
		
		const vector<const Letter*>& GetLetters() const;

		//Set the font metrics upscale value for all font and per-glyph values,
		//clamped internally from 0.01f to 10.0f
		void SetUpscale(f32 newValue);
		f32 GetUpscale() const;
		
		void SetColor(const vec3& newValue);
		const vec3& GetColor() const;
		
		void SetOpacity(float newValue);
		f32 GetOpacity() const;
		
		void SetUnderline(bool newValue);
		bool IsUnderlineEnabled() const;
		
		void SetStrikethrough(bool newValue);
		bool IsStrikethroughEnabled() const;

		void SetFontID(u32 newValue);
		u32 GetFontID() const;

		//Cleans VAO, VBO and EBO, should be called just before gl context shutdown
		static void CleanStaticResources();

		//Do not destroy manually, erase from registry instead
		virtual ~OpenGL_Text() override;
	protected:
		virtual void UpdateAABB(f32 viewportHeight);
	private:	
		static void CreateTextGeometry();

		vector<u32> text{};              //the text typed by the user
		vector<const Letter*> letters{}; //all the letters that have been typed to this text

		f32 fontMetricsUpscale = 2.0f;
		
		vec3 color{};         //what color the visible letters are
		f32 opacity{};        //how see-through the visible letters are
		bool underline{};     //should this text have an underline
		bool strikethrough{}; //should this text be striked through
		
		u32 fontID{};
	};
}