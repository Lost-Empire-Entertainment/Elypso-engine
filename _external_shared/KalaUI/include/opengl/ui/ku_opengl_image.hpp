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

		u32 GetVAO() const;
		u32 GetVBO() const;
		u32 GetEBO() const;

		void SetVertices(const vector<vec2>& newVertices);
		void SetIndices(const vector<u32>& newIndices);
		void SetUVs(const vector<u32>& newUVs);

		const vector<vec2>& GetVertices() const;
		const vector<u32>& GetIndices() const;
		const vector<u32>& GetUVs() const;

		void SetTextureID(u32 newTexture);
		void ClearTextureID();
		const u32 GetTextureID() const;

		//Do not destroy manually, erase from registry instead
		virtual ~OpenGL_Image() override;
	protected:
		virtual void UpdateAABB(f32 viewportHeight);

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
			0, 1,
			1, 1,
			1, 0,
			0, 0
		};

		u32 textureID{};
	};
}