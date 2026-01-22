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
		//state

		PFNGLENABLEPROC      glEnable;
		PFNGLDISABLEPROC     glDisable;
		PFNGLBLENDFUNCPROC   glBlendFunc;
		PFNGLDEPTHMASKPROC   glDepthMask;
		PFNGLGETERRORPROC    glGetError;
		PFNGLGETINTEGERVPROC glGetIntegerv;

		//buffers / VAOs

		PFNGLGENBUFFERSPROC      glGenBuffers;
		PFNGLDELETEBUFFERSPROC   glDeleteBuffers;
		PFNGLBINDBUFFERPROC      glBindBuffer;
		PFNGLBUFFERDATAPROC      glBufferData;

		PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
		PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
		PFNGLBINDVERTEXARRAYPROC    glBindVertexArray;

		PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
		PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
		PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

		//drawing

		PFNGLDRAWELEMENTSPROC glDrawElements;

		//shaders / programs

		PFNGLCREATESHADERPROC      glCreateShader;
		PFNGLSHADERSOURCEPROC      glShaderSource;
		PFNGLCOMPILESHADERPROC     glCompileShader;
		PFNGLGETSHADERIVPROC       glGetShaderiv;
		PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog;
		PFNGLDELETESHADERPROC      glDeleteShader;

		PFNGLCREATEPROGRAMPROC     glCreateProgram;
		PFNGLATTACHSHADERPROC      glAttachShader;
		PFNGLDETACHSHADERPROC      glDetachShader;
		PFNGLLINKPROGRAMPROC       glLinkProgram;
		PFNGLVALIDATEPROGRAMPROC   glValidateProgram;
		PFNGLUSEPROGRAMPROC        glUseProgram;
		PFNGLISPROGRAMPROC         glIsProgram;
		PFNGLGETPROGRAMIVPROC      glGetProgramiv;
		PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
		PFNGLDELETEPROGRAMPROC     glDeleteProgram;

		PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
		PFNGLUNIFORM1IPROC          glUniform1i;
		PFNGLUNIFORM1FPROC          glUniform1f;
		PFNGLUNIFORM1FVPROC         glUniform1fv;
		PFNGLUNIFORM2FVPROC         glUniform2fv;
		PFNGLUNIFORM3FVPROC         glUniform3fv;
		PFNGLUNIFORM4FVPROC         glUniform4fv;
		PFNGLUNIFORMMATRIX2FVPROC   glUniformMatrix2fv;
		PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv;
		PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;

		//textures

		PFNGLACTIVETEXTUREPROC   glActiveTexture;
		PFNGLGENTEXTURESPROC     glGenTextures;
		PFNGLDELETETEXTURESPROC  glDeleteTextures;
		PFNGLBINDTEXTUREPROC     glBindTexture;
		PFNGLTEXPARAMETERIPROC   glTexParameteri;
		PFNGLPIXELSTOREIPROC     glPixelStorei;
		PFNGLTEXIMAGE2DPROC      glTexImage2D;
		PFNGLTEXSUBIMAGE2DPROC   glTexSubImage2D;
		PFNGLTEXSUBIMAGE3DPROC   glTexSubImage3D;
		PFNGLTEXSTORAGE2DPROC    glTexStorage2D;
		PFNGLTEXSTORAGE3DPROC    glTexStorage3D;
		PFNGLGENERATEMIPMAPPROC  glGenerateMipmap;
		PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
	};

	class LIB_API OpenGL_Functions_UI
	{
	public:
		static void SetGLUI(const GL_UI* gl);
		static const GL_UI* GetGLUI();
	};
}