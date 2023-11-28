#pragma once

//engine
#include "writeConsoleMessage.h"

//external
#include "glad.h"
#include "glfw3.h"

#include <iostream>
#include <string>

enum ShaderState {
	vertex,
	fragment,
	link_shaders,
	vertex_data_and_buffers
};

extern GLuint framebuffer = 0;
extern GLuint textureColorbuffer = 0;

int WindowSetup();
void ImGuiSetup();

void SetUpVertexShader();
void SetUpFragmentShader();
void LinkShaders();
void SetUpVertexDataAndBuffers();
bool FoundShaderCompileErrors(ShaderState state);

void RenderUI();
void RenderToTexture();
void WindowLoop();
void Shutdown(MessageType reason, ErrorType errorType, const std::string& errorMessage);

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);