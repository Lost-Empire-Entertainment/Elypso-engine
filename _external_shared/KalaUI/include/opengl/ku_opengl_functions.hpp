//Copyright(C) 2025 Lost Empire Entertainment
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
	//general stuff
		
	LIB_API extern PFNGLENABLEPROC             glEnable;
	LIB_API extern PFNGLDISABLEPROC            glDisable;
	LIB_API extern PFNGLBLENDFUNCPROC          glBlendFunc;
	LIB_API extern PFNGLDEPTHMASKPROC          glDepthMask;
	LIB_API extern PFNGLBUFFERDATAPROC         glBufferData;
	LIB_API extern PFNGLDRAWELEMENTSPROC       glDrawElements;
	LIB_API extern PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
	LIB_API extern PFNGLGENBUFFERSPROC         glGenBuffers;
	LIB_API extern PFNGLBINDBUFFERPROC         glBindBuffer;
	LIB_API extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
	LIB_API extern PFNGLDELETEBUFFERSPROC      glDeleteBuffers;
	LIB_API extern PFNGLGETERRORPROC           glGetError;
		
	//shader stuff
		
	LIB_API extern PFNGLCREATESHADERPROC       glCreateShader;
	LIB_API extern PFNGLCREATEPROGRAMPROC      glCreateProgram;
	LIB_API extern PFNGLVALIDATEPROGRAMPROC    glValidateProgram;
	LIB_API extern PFNGLATTACHSHADERPROC       glAttachShader;
	LIB_API extern PFNGLLINKPROGRAMPROC        glLinkProgram;
	LIB_API extern PFNGLGETPROGRAMIVPROC       glGetProgramiv;
	LIB_API extern PFNGLGETPROGRAMINFOLOGPROC  glGetProgramInfoLog;
	LIB_API extern PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog;
	LIB_API extern PFNGLISPROGRAMPROC          glIsProgram;
	LIB_API extern PFNGLUSEPROGRAMPROC         glUseProgram;
	LIB_API extern PFNGLSHADERSOURCEPROC       glShaderSource;
	LIB_API extern PFNGLCOMPILESHADERPROC      glCompileShader;
	LIB_API extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	LIB_API extern PFNGLGETINTEGERVPROC        glGetIntegerv;
	LIB_API extern PFNGLGETSHADERIVPROC        glGetShaderiv;
	LIB_API extern PFNGLUNIFORM1IPROC          glUniform1i;
	LIB_API extern PFNGLUNIFORM1FPROC          glUniform1f;
	LIB_API extern PFNGLUNIFORM1FVPROC         glUniform1fv;
	LIB_API extern PFNGLUNIFORM2FVPROC         glUniform2fv;
	LIB_API extern PFNGLUNIFORM3FVPROC         glUniform3fv;
	LIB_API extern PFNGLUNIFORM4FVPROC         glUniform4fv;
	LIB_API extern PFNGLUNIFORMMATRIX2FVPROC   glUniformMatrix2fv;
	LIB_API extern PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv;
	LIB_API extern PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;
	LIB_API extern PFNGLDETACHSHADERPROC       glDetachShader;
	LIB_API extern PFNGLDELETESHADERPROC       glDeleteShader;
	LIB_API extern PFNGLDELETEPROGRAMPROC      glDeleteProgram;
		
	//texture stuff
		
	LIB_API extern PFNGLACTIVETEXTUREPROC           glActiveTexture;
	LIB_API extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	LIB_API extern PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
	LIB_API extern PFNGLBINDTEXTUREPROC             glBindTexture;
	LIB_API extern PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
	LIB_API extern PFNGLGENTEXTURESPROC             glGenTextures;
	LIB_API extern PFNGLTEXPARAMETERIPROC           glTexParameteri;
	LIB_API extern PFNGLPIXELSTOREIPROC             glPixelStorei;
	LIB_API extern PFNGLTEXIMAGE2DPROC              glTexImage2D;
	LIB_API extern PFNGLTEXSUBIMAGE2DPROC           glTexSubImage2D;
	LIB_API extern PFNGLTEXSTORAGE2DPROC            glTexStorage2D;
	LIB_API extern PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;
	LIB_API extern PFNGLDELETETEXTURESPROC          glDeleteTextures;
}