//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

#include "graphics/kw_window_global.hpp"
#include "opengl/kw_opengl.hpp"
#include "vulkan/kw_vulkan.hpp"
#include "core/kg_context.hpp"
#include "_internal/opengl/_kg_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"
#ifdef _WIN32
#include "opengl/kw_opengl_functions_windows.hpp"
#else
#include "opengl/kw_opengl_functions_linux.hpp"
#include "core/kw_messageloop_x11.hpp"
#endif

#include "graphics/ee_render.hpp"
#include "graphics/ee_window.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Graphics::Window_Global;
using KalaWindow::OpenGL::OpenGL_Global;
using KalaWindow::Vulkan::Vulkan_Global;
using KalaGraphics::Core::WindowContext;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Core;
using KalaGraphics::Internal::OpenGL::OpenGL_Core;
using KalaGraphics::Internal::OpenGL::OpenGL_Core_Functions;
#ifdef _WIN32
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Windows;
using KalaGraphics::Internal::OpenGL::OpenGL_Windows_Functions;
#else
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Linux;
using KalaGraphics::Internal::OpenGL::OpenGL_Linux_Functions;
using KalaWindow::Core::MessageLoop;
#endif

namespace ElypsoEngine::Graphics
{
    static bool isInitialized{};
    static bool allowGL{};
    static bool allowVK{};

    void Render::Initialize(
        bool newAllowGL,
        bool newAllowVK)
    {
        if (isInitialized)
        {
            Log::Print(
                "Cannot initialize Elypso Engine render structure more than once!",
                "ELYPSO_RENDER",
                LogType::LOG_ERROR,
                2);

            return;
        }

        allowGL = newAllowGL;
        allowVK = newAllowVK;

        Window_Global::Initialize();
        if (allowGL)
        {
            OpenGL_Global::Initialize();

            auto* glCore = OpenGL_Functions_Core::GetGLCore();
            OpenGL_Core::SetCoreFunctions(
                {
                    .glMapBufferRange           = glCore->glMapBufferRange,
                    .glBufferStorage            = glCore->glBufferStorage,
                    .glBindBuffer               = glCore->glBindBuffer,
                    .glBindVertexArray          = glCore->glBindVertexArray,
                    .glBufferData               = glCore->glBufferData,
                    .glBufferSubData            = glCore->glBufferSubData,
                    .glDeleteBuffers            = glCore->glDeleteBuffers,
                    .glDeleteVertexArrays       = glCore->glDeleteVertexArrays,
                    .glDrawArrays               = glCore->glDrawArrays,
                    .glDrawElements             = glCore->glDrawElements,
                    .glEnableVertexAttribArray  = glCore->glEnableVertexAttribArray,
                    .glGenBuffers               = glCore->glGenBuffers,
                    .glGenVertexArrays          = glCore->glGenVertexArrays,
                    .glGetVertexAttribiv        = glCore->glGetVertexAttribiv,
                    .glGetVertexAttribPointerv  = glCore->glGetVertexAttribPointerv,
                    .glVertexAttribPointer      = glCore->glVertexAttribPointer,
                    .glDisableVertexAttribArray = glCore->glDisableVertexAttribArray,
                    .glVertexAttribI1i          = glCore->glVertexAttribI1i,
                    .glVertexAttribI2i          = glCore->glVertexAttribI2i,
                    .glVertexAttribI3i          = glCore->glVertexAttribI3i,
                    .glVertexAttribI4i          = glCore->glVertexAttribI4i,
                    .glCullFace                 = glCore->glCullFace,

                    .glAttachShader      = glCore->glAttachShader,
                    .glCompileShader     = glCore->glCompileShader,
                    .glCreateProgram     = glCore->glCreateProgram,
                    .glCreateShader      = glCore->glCreateShader,
                    .glDeleteShader      = glCore->glDeleteShader,
                    .glDeleteProgram     = glCore->glDeleteProgram,
                    .glDetachShader      = glCore->glDetachShader,
                    .glGetActiveAttrib   = glCore->glGetActiveAttrib,
                    .glGetAttribLocation = glCore->glGetAttribLocation,
                    .glGetProgramiv      = glCore->glGetProgramiv,
                    .glGetProgramInfoLog = glCore->glGetProgramInfoLog,
                    .glGetShaderiv       = glCore->glGetShaderiv,
                    .glGetShaderInfoLog  = glCore->glGetShaderInfoLog,
                    .glLinkProgram       = glCore->glLinkProgram,
                    .glShaderSource      = glCore->glShaderSource,
                    .glUseProgram        = glCore->glUseProgram,
                    .glValidateProgram   = glCore->glValidateProgram,
                    .glIsProgram         = glCore->glIsProgram,

                    .glGetUniformLocation   = glCore->glGetUniformLocation,
                    .glGetUniformBlockIndex = glCore->glGetUniformBlockIndex,
                    .glUniformBlockBinding  = glCore->glUniformBlockBinding,
                    .glUniform1f            = glCore->glUniform1f,
                    .glUniform1i            = glCore->glUniform1i,
                    .glUniform1fv           = glCore->glUniform1fv,
                    .glUniform1iv           = glCore->glUniform1iv,
                    .glUniform2f            = glCore->glUniform2f,
                    .glUniform2i            = glCore->glUniform2i,
                    .glUniform2fv           = glCore->glUniform2fv,
                    .glUniform2iv           = glCore->glUniform2iv,
                    .glUniform3f            = glCore->glUniform3f,
                    .glUniform3i            = glCore->glUniform3i,
                    .glUniform3fv           = glCore->glUniform3fv,
                    .glUniform3iv           = glCore->glUniform3iv,
                    .glUniform4f            = glCore->glUniform4f,
                    .glUniform4i            = glCore->glUniform4i,
                    .glUniform4fv           = glCore->glUniform4fv,
                    .glUniform4iv           = glCore->glUniform4iv,
                    .glUniformMatrix2fv     = glCore->glUniformMatrix2fv,
                    .glUniformMatrix3fv     = glCore->glUniformMatrix3fv,
                    .glUniformMatrix4fv     = glCore->glUniformMatrix4fv
                });

#ifdef _WIN32
            OpenGL_Core::SetWindowsFunctions(
                {
                    .wglSwapIntervalEXT = OpenGL_Functions_Windows::GetGLWindows()->wglSwapIntervalEXT
                });
#else
            OpenGL_Core::SetLinuxFunctions(
                {
                    .glXSwapIntervalEXT = OpenGL_Functions_Linux::GetGLLinux()->glXSwapIntervalEXT
                });
#endif
        }
        if (allowVK) Vulkan_Global::Initialize();

        Log::Print(
            "Initialized render!",
            "ELYPSO_RENDER",
            LogType::LOG_SUCCESS);
    }

    bool Render::IsInitialized() { return isInitialized; }

    bool Render::AllowGL() { return allowGL; }
    bool Render::AllowVK() { return allowVK; }

    void Render::Update()
    {
        if (EngineWindow::GetRegistry().runtimeContent.empty())
        {
            Log::Print(
                "There are no windows to render! Did you forget to initialize a window?",
                "ELYPSO_RENDER",
                LogType::LOG_WARNING);

            return;
        }

#ifdef __linux__
        MessageLoop::Update();
#endif

        for (const auto& w : EngineWindow::GetRegistry().runtimeContent)
        {
            w->Update();
        }
    }

    void Render::Shutdown()
    {
        Log::Print(
            "Shutting down Elypso Engine render structure.",
            "ELYPSO_RENDER",
            LogType::LOG_INFO);

        for (const auto& w : EngineWindow::GetRegistry().runtimeContent)
        {
            w->Shutdown();
        }
    }
}