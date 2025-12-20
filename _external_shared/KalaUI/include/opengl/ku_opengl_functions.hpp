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

namespace KalaUI::OpenGL::OpenGL_Functions
{
	//general stuff
		
	LIB_API inline PFNGLENABLEPROC             glEnable{};
	LIB_API inline PFNGLDISABLEPROC            glDisable{};
	LIB_API inline PFNGLBLENDFUNCPROC          glBlendFunc{};
	LIB_API inline PFNGLDEPTHMASKPROC          glDepthMask{};
	LIB_API inline PFNGLBUFFERDATAPROC         glBufferData{};
	LIB_API inline PFNGLDRAWELEMENTSPROC       glDrawElements{};
	LIB_API inline PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays{};
	LIB_API inline PFNGLGENBUFFERSPROC         glGenBuffers{};
	LIB_API inline PFNGLBINDBUFFERPROC         glBindBuffer{};
	LIB_API inline PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays{};
	LIB_API inline PFNGLDELETEBUFFERSPROC      glDeleteBuffers{};
	LIB_API inline PFNGLGETERRORPROC           glGetError{};
		
	//shader stuff
		
	LIB_API inline PFNGLCREATESHADERPROC       glCreateShader{};
	LIB_API inline PFNGLCREATEPROGRAMPROC      glCreateProgram{};
	LIB_API inline PFNGLVALIDATEPROGRAMPROC    glValidateProgram{};
	LIB_API inline PFNGLATTACHSHADERPROC       glAttachShader{};
	LIB_API inline PFNGLLINKPROGRAMPROC        glLinkProgram{};
	LIB_API inline PFNGLGETPROGRAMIVPROC       glGetProgramiv{};
	LIB_API inline PFNGLGETPROGRAMINFOLOGPROC  glGetProgramInfoLog{};
	LIB_API inline PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog{};
	LIB_API inline PFNGLISPROGRAMPROC          glIsProgram{};
	LIB_API inline PFNGLUSEPROGRAMPROC         glUseProgram{};
	LIB_API inline PFNGLSHADERSOURCEPROC       glShaderSource{};
	LIB_API inline PFNGLCOMPILESHADERPROC      glCompileShader{};
	LIB_API inline PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation{};
	LIB_API inline PFNGLGETINTEGERVPROC        glGetIntegerv{};
	LIB_API inline PFNGLGETSHADERIVPROC        glGetShaderiv{};
	LIB_API inline PFNGLUNIFORM1IPROC          glUniform1i{};
	LIB_API inline PFNGLUNIFORM1FPROC          glUniform1f{};
	LIB_API inline PFNGLUNIFORM1FVPROC         glUniform1fv{};
	LIB_API inline PFNGLUNIFORM2FVPROC         glUniform2fv{};
	LIB_API inline PFNGLUNIFORM3FVPROC         glUniform3fv{};
	LIB_API inline PFNGLUNIFORM4FVPROC         glUniform4fv{};
	LIB_API inline PFNGLUNIFORMMATRIX2FVPROC   glUniformMatrix2fv{};
	LIB_API inline PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv{};
	LIB_API inline PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv{};
	LIB_API inline PFNGLDETACHSHADERPROC       glDetachShader{};
	LIB_API inline PFNGLDELETESHADERPROC       glDeleteShader{};
	LIB_API inline PFNGLDELETEPROGRAMPROC      glDeleteProgram{};
		
	//texture stuff
		
	LIB_API inline PFNGLACTIVETEXTUREPROC           glActiveTexture{};
	LIB_API inline PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray{};
	LIB_API inline PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer{};
	LIB_API inline PFNGLBINDTEXTUREPROC             glBindTexture{};
	LIB_API inline PFNGLBINDVERTEXARRAYPROC         glBindVertexArray{};
	LIB_API inline PFNGLGENTEXTURESPROC             glGenTextures{};
	LIB_API inline PFNGLTEXPARAMETERIPROC           glTexParameteri{};
	LIB_API inline PFNGLPIXELSTOREIPROC             glPixelStorei{};
	LIB_API inline PFNGLTEXIMAGE2DPROC              glTexImage2D{};
	LIB_API inline PFNGLTEXSUBIMAGE2DPROC           glTexSubImage2D{};
	LIB_API inline PFNGLTEXSTORAGE2DPROC            glTexStorage2D{};
	LIB_API inline PFNGLGENERATEMIPMAPPROC          glGenerateMipmap{};
	LIB_API inline PFNGLDELETETEXTURESPROC          glDeleteTextures{};
}