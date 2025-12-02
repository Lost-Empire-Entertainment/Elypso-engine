//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/math_utils.hpp"

#include "opengl/ui/kg_opengl_widget.hpp"

namespace KalaGraphics::OpenGL::UI
{
	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaMath::mat3;

	class LIB_API OpenGL_Image : public OpenGL_Widget
	{
	public:
		//Initialize a new Image widget.
		//Parent widget is optional
		static OpenGL_Image* Initialize(
			const string& name,
			u32 windowID,
			u32 glID,
			const vec2 pos,
			const float rot,
			const vec2 size,
			OpenGL_Texture* texture,
			OpenGL_Shader* shader,
			OpenGL_Widget* parentWidget = {});

		//Render this image widget. Requires handle (HDC) from your window
		virtual bool Render(
			uintptr_t handle,
			const mat4& projection,
			f32 viewportHeight) override;

		//Do not destroy manually, erase from registry instead
		virtual ~OpenGL_Image() override;
	protected:
		virtual void UpdateAABB(f32 viewportHeight);
	};
}