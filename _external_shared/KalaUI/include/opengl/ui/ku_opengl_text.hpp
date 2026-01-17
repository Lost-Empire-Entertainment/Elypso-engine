//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/import_kfd.hpp"

#include "opengl/ui/ku_opengl_widget.hpp"

namespace KalaUI::OpenGL
{
	class OpenGL_Manager; //forward-declare the manager
}

namespace KalaUI::OpenGL::UI
{
	using KalaHeaders::KalaMath::kclamp;
	using KalaHeaders::KalaFontData::GlyphBlock;
	
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
			u32 glyphIndex,
			u32 fontID,
			const vec2 pos,
			const float rot,
			float sizeMultiplier,
			OpenGL_Texture* texture,
			OpenGL_Shader* shader,
			OpenGL_Widget* parentWidget = {});
			
		virtual bool Render(
			const mat4& projection,
			f32 viewportHeight) override;

		void AddChar(u32 newValue);
		void AddTab();
		void AddNewLine();
		void RemoveCharFromBack();
		
		void SetText(const vector<u32>& newValue);
		vector<u32>& GetText();
		
		void SetLetters(const vector<GlyphBlock*>& newValue);
		vector<GlyphBlock*> GetLetters() const;
		
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

		//Do not destroy manually, erase from registry instead
		virtual ~OpenGL_Text() override;
	protected:
		virtual void UpdateAABB(f32 viewportHeight);
	private:	
		vector<u32> text{};            //the text typed by the user
		vector<GlyphBlock*> letters{}; //all the letters that have been typed to this text
		
		vec3 color{};         //what color the visible letters are
		f32 opacity{};        //how see-through the visible letters are
		bool underline{};     //should this text have an underline
		bool strikethrough{}; //should this text be striked through
		
		u32 fontID{};
		u32 textureID{};
	};
}