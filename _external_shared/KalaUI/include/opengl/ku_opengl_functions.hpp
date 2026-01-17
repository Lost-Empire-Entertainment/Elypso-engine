//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "OpenGL/glcorearb.h" //core opengl
#include "OpenGL/glext.h"     //extension tokens, enums and extra function pointers

#include "KalaHeaders/core_utils.hpp"

//
// IMPORTANT NOTE:
//
// KalaUI does not own these functions,
// their ownership must come from another library, for example KalaWindow
//

namespace KalaUI::OpenGL::OpenGLFunctions
{
	struct GL_UI
	{
		//general stuff

		PFNGLENABLEPROC             glEnable;
		PFNGLDISABLEPROC            glDisable;
		PFNGLBLENDFUNCPROC          glBlendFunc;
		PFNGLDEPTHMASKPROC          glDepthMask;
		PFNGLBUFFERDATAPROC         glBufferData;
		PFNGLDRAWELEMENTSPROC       glDrawElements;
		PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
		PFNGLGENBUFFERSPROC         glGenBuffers;
		PFNGLBINDBUFFERPROC         glBindBuffer;
		PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
		PFNGLDELETEBUFFERSPROC      glDeleteBuffers;
		PFNGLGETERRORPROC           glGetError;

		//shader stuff

		PFNGLCREATESHADERPROC       glCreateShader;
		PFNGLCREATEPROGRAMPROC      glCreateProgram;
		PFNGLVALIDATEPROGRAMPROC    glValidateProgram;
		PFNGLATTACHSHADERPROC       glAttachShader;
		PFNGLLINKPROGRAMPROC        glLinkProgram;
		PFNGLGETPROGRAMIVPROC       glGetProgramiv;
		PFNGLGETPROGRAMINFOLOGPROC  glGetProgramInfoLog;
		PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog;
		PFNGLISPROGRAMPROC          glIsProgram;
		PFNGLUSEPROGRAMPROC         glUseProgram;
		PFNGLSHADERSOURCEPROC       glShaderSource;
		PFNGLCOMPILESHADERPROC      glCompileShader;
		PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
		PFNGLGETINTEGERVPROC        glGetIntegerv;
		PFNGLGETSHADERIVPROC        glGetShaderiv;
		PFNGLUNIFORM1IPROC          glUniform1i;
		PFNGLUNIFORM1FPROC          glUniform1f;
		PFNGLUNIFORM1FVPROC         glUniform1fv;
		PFNGLUNIFORM2FVPROC         glUniform2fv;
		PFNGLUNIFORM3FVPROC         glUniform3fv;
		PFNGLUNIFORM4FVPROC         glUniform4fv;
		PFNGLUNIFORMMATRIX2FVPROC   glUniformMatrix2fv;
		PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv;
		PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;
		PFNGLDETACHSHADERPROC       glDetachShader;
		PFNGLDELETESHADERPROC       glDeleteShader;
		PFNGLDELETEPROGRAMPROC      glDeleteProgram;

		//texture stuff

		PFNGLACTIVETEXTUREPROC           glActiveTexture;
		PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
		PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
		PFNGLBINDTEXTUREPROC             glBindTexture;
		PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
		PFNGLGENTEXTURESPROC             glGenTextures;
		PFNGLTEXPARAMETERIPROC           glTexParameteri;
		PFNGLPIXELSTOREIPROC             glPixelStorei;
		PFNGLTEXIMAGE2DPROC              glTexImage2D;
		PFNGLTEXSUBIMAGE2DPROC           glTexSubImage2D;
		PFNGLTEXSTORAGE2DPROC            glTexStorage2D;
		PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;
		PFNGLDELETETEXTURESPROC          glDeleteTextures;
	};

	class LIB_API OpenGL_Functions_UI
	{
	public:
		static void SetGLUI(const GL_UI* gl);
		static const GL_UI* GetGLUI();
	};
}