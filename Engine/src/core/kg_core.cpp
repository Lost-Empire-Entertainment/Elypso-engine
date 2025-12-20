//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>

#include "KalaHeaders/log_utils.hpp"

#include "core/kg_core.hpp"
#include "core/kg_font.hpp"
#include "core/kg_camera.hpp"
#include "graphics/kg_opengl_texture.hpp"
#include "graphics/kg_opengl_shader.hpp"
#include "ui/kg_opengl_widget.hpp"
#include "gameobject/kg_opengl_model.hpp"
#include "gameobject/kg_opengl_point_light.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaGL::Graphics::OpenGL_Shader;
using KalaGL::Graphics::OpenGL_Texture;
using KalaGL::UI::OpenGL_Widget;
using KalaGL::GameObject::OpenGL_Model;
using KalaGL::GameObject::OpenGL_PointLight;

using std::to_string;

namespace KalaGL::Core
{
	void KalaGLCore::CleanAllWindowResources(u32 windowID)
	{
		Log::Print(
			"Cleaning resources for window '" + to_string(windowID) + "'.",
			"KALAGL",
			LogType::LOG_INFO);
		
		OpenGL_Widget::registry.RemoveAllWindowContent(windowID);
	}
	
	void KalaGLCore::CleanAllResources()
	{
		Log::Print(
			"Cleaning all KalaGL resources.",
			"KALAGL",
			LogType::LOG_INFO);
		
		OpenGL_Widget::registry.RemoveAllContent();
		Font::registry.RemoveAllContent();
		
		Camera::registry.RemoveAllContent();
		
		OpenGL_Model::registry.RemoveAllContent();
		OpenGL_PointLight::registry.RemoveAllContent();
		OpenGL_Texture::registry.RemoveAllContent();
		OpenGL_Shader::registry.RemoveAllContent();
	}
}