//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/math_utils.hpp"

#include "opengl/ui/ku_opengl_widget.hpp"

namespace KalaUI::OpenGL
{
	class OpenGL_Manager; //forward-declare the manager
}

namespace KalaUI::OpenGL::UI
{
	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaMath::mat3;

	class LIB_API OpenGL_Image : public OpenGL_Widget
	{
		friend class OpenGL_Manager; //friend-include the manager
	public:
		//Initialize a new Image widget.
		//Parent widget is optional
		static OpenGL_Image* Initialize(
			const string& name,
			u32 windowID,
			uintptr_t glContext,
			const vec2 pos,
			const float rot,
			const vec2 size,
			OpenGL_Texture* texture,
			OpenGL_Shader* shader,
			OpenGL_Widget* parentWidget = {});
			
		virtual bool Render(
			const mat4& projection,
			f32 viewportHeight) override;

		void SetTextureID(u32 newTexture);
		void ClearTextureID();
		const u32 GetTextureID() const;

		//Cleans VAO, VBO and EBO, should be called just before gl context shutdown
		static void CleanStaticResources();

		//Do not destroy manually, erase from registry instead
		virtual ~OpenGL_Image() override;
	protected:
		virtual void UpdateAABB(f32 viewportHeight);

		u32 textureID{};
	private:
		static void CreateImageGeometry();
	};
}