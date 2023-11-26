#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

enum MessageType {
	SUCCESS,
	SHADER_ERROR,
	WINDOW_SETUP_ERROR,
	GLFW_ERROR,
	GLAD_ERROR,
	USER_CLOSE
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int WindowSetup();
void ShaderSetup();
void WindowLoop();
void WriteConsoleMessage(MessageType messageType, const std::string& message);
void Shutdown(MessageType reason, const std::string& errorMessage);

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
