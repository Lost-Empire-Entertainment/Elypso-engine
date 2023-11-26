#include "main.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <magic_enum/magic_enum.hpp>

#include <iostream>
#include <string>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int VAO, VBO, shaderProgram;

GLFWwindow* window;

const char *vertexShaderSource = 
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";
const char *fragmentShaderSource =
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";

int main()
{
	if (WindowSetup() != 0) {
		Shutdown(WINDOW_SETUP_ERROR, "Window setup failed.");
		return -1;
	}
	ShaderSetup();

	WriteConsoleMessage(SUCCESS, "Setup was successful!");

	//keep window open until user closes it
	while (!glfwWindowShouldClose(window)) WindowLoop();

	Shutdown(USER_CLOSE, "User closed program.");
	return 0;
}

static int WindowSetup()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create a window object holding all the windowing data
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Elypso engine", NULL, NULL);
	if (window == NULL)
	{
		Shutdown(GLFW_ERROR, "Failed to create GLFW window");
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//check if glad is initialized before continuing
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Shutdown(GLAD_ERROR, "Failed to initialize GLAD");
		return -1;
	}

	return 0;
}

static void ShaderSetup() 
{
	//vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		Shutdown(SHADER_ERROR, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(infoLog));
	}

	//fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		Shutdown(SHADER_ERROR, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" + std::string(infoLog));
	}

	//link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		Shutdown(SHADER_ERROR, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog));
	}

	//clean up shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//set up vertex data and buffers and configure vertex attributes
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, //left
		0.5f, -0.5f, 0.0f,  //right
		0.0f, 0.5f, 0.0f    //top
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//bind the Vertex Array Object first, 
	//then bind and set vertex buffers, 
	//and then configure vertex attributes
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//the call to glVertexAttribPointer registered VBO
	//as the vertex attributes bound vertex buffer object
	//so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//unbind the VAO afterwards so other VAO calls wont accidentally modify this VAO,
	//but this rarely happens. modifying other VAOs requires a call to glBindVertexArray anyways
	//so we generally dont unbind VAOs (nor VBOs) when its not directly necessary
	glBindVertexArray(0);

	//uncomment this call to draw in wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

static void WindowLoop() 
{
	processInput(window);

	//simple green color instead of black
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//draw the triangle
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

static void WriteConsoleMessage(MessageType messageType, const std::string& message)
{
	std::cout << "[" << magic_enum::enum_name(messageType) << "] " << message;
}

static void Shutdown(MessageType reason, const std::string& errorMessage)
{
	WriteConsoleMessage(reason, errorMessage);

	switch (reason) 
	{
		case SHADER_ERROR:
			break;
		case GLFW_ERROR:
			glfwTerminate();
			break;
		case GLAD_ERROR:
			break;
		case USER_CLOSE:
			//de-allocate all resources once they've outlived their purpose
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteProgram(shaderProgram);

			//clean all glfw resources after program is closed
			glfwTerminate();
			break;
	}
}

//process all input and react accordingly
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

//make sure the viewport matches the new window dimentions
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}