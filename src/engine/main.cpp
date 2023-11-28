//engine
#include <main.h>
#include "timestamp.h"
#include "writeConsoleMessage.h"

//external
#include "glad.h"
#include "glfw3.h"
#include "magic_enum.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <string>
#include <sstream>

bool shaderSetupSuccess;

int success;
char infoLog[512];

unsigned int VAO, VBO, shaderProgram, vertexShader, fragmentShader;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLenum error;
GLFWwindow* window;

const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main()
{
	WriteConsoleMessage(ENGINE, INFO, "Initializing Elypso engine...\n");

	if (WindowSetup() != 0) {
		Shutdown(WINDOW_SETUP, ERROR, "Error: Window setup failed!\n\n");
		return -1;
	}

	ImGuiSetup();

	//starts shader setup
	SetUpVertexShader();

	if (!shaderSetupSuccess)
	{
		WriteConsoleMessage(SHADER, ERROR, "Shader setup was unsuccessful!");
		return -1;
	}
	else 
	{
		WriteConsoleMessage(WINDOW_LOOP, INFO, "Entering window loop...\n");

		//keep window open until user closes it
		while (!glfwWindowShouldClose(window)) WindowLoop();

		WriteConsoleMessage(WINDOW_LOOP, INFO, "Exiting window loop...\n");

		Shutdown(SHUTDOWN, INFO, "Shutting down Elypso engine...\n\n");
		return 0;
	}
}

//sets up glfw, glad and the window of the application
static int WindowSetup()
{
	WriteConsoleMessage(GLFW, INFO, "Initializing GLFW...\n");

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	WriteConsoleMessage(GLFW, SUCCESS, "GLFW initialized successfully!\n\n");

	WriteConsoleMessage(WINDOW_SETUP, INFO, "Creating window...\n");

	//create a window object holding all the windowing data
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Elypso engine", NULL, NULL);
	if (window == NULL)
	{
		Shutdown(GLFW, ERROR, "Error: Failed to create GLFW window!\n\n");
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	WriteConsoleMessage(WINDOW_SETUP, SUCCESS, "Window initialized successfully!\n\n");

	WriteConsoleMessage(GLAD, INFO, "Initializing GLAD...\n");

	//check if glad is initialized before continuing
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		WriteConsoleMessage(GLAD, ERROR, "Error: Failed to initialize GLAD!\n\n");
		return -1;
	}

	WriteConsoleMessage(GLAD, SUCCESS, "GLAD initialized successfully!\n\n");

	return 0;
}

static void ImGuiSetup()
{
	WriteConsoleMessage(IMGUI, INFO, "Initializing ImGui...\n");

	//set up imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	//initialize imgui for glfw and opengl
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	WriteConsoleMessage(IMGUI, SUCCESS, "ImGui initialized successfully!\n\n");
}

static void SetUpVertexShader() 
{
	WriteConsoleMessage(SHADER, INFO, "Initializing vertex shader...\n");

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	if (!FoundShaderCompileErrors(vertex))
	{
		WriteConsoleMessage(SHADER, SUCCESS, "Vertex shader initialized successfully!\n\n");
	}
	else
	{
		WriteConsoleMessage(SHADER, ERROR, "Vertex shader compilation failed!\n\n");
		return;
	}

	SetUpFragmentShader();
}
static void SetUpFragmentShader() 
{
	WriteConsoleMessage(SHADER, INFO, "Initializing fragment shader...\n");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	if (!FoundShaderCompileErrors(fragment))
	{
		WriteConsoleMessage(SHADER, SUCCESS, "Fragment shader initialized successfully!\n\n");
	}
	else
	{
		WriteConsoleMessage(SHADER, ERROR, "Fragment shader compilation failed!\n\n");
		return;
	}

	LinkShaders();
}
static void LinkShaders() 
{
	WriteConsoleMessage(SHADER, INFO, "Linking shaders...\n");

	//link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//clean up shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (!FoundShaderCompileErrors(link_shaders))
	{
		WriteConsoleMessage(SHADER, SUCCESS, "Shaders linked successfully!\n\n");
	}
	else
	{
		WriteConsoleMessage(SHADER, ERROR, "Shader linking failed!\n\n");
		return;
	}

	SetUpVertexDataAndBuffers();
}
static void SetUpVertexDataAndBuffers() 
{
	WriteConsoleMessage(SHADER, INFO, "Setting up vertex data and buffers...\n");

	//enable depth testing
	glEnable(GL_DEPTH_TEST);

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

	if (!FoundShaderCompileErrors(vertex_data_and_buffers))
	{
		WriteConsoleMessage(SHADER, SUCCESS, "Vertex data and buffer setup was successful!\n\n");
	}
	else
	{
		WriteConsoleMessage(SHADER, ERROR, "Failed to set up vertex data and buffers!\n\n");
		return;
	}

	shaderSetupSuccess = true;
}

//checks if any errors were found during compilation
static bool FoundShaderCompileErrors(ShaderState state) 
{
	std::ostringstream oss;
	std::string message;

	switch (state) 
	{
	default:
		oss << "Error: " << magic_enum::enum_name(state) << " is not a valid shader state!\n\n";
		WriteConsoleMessage(ENGINE, ERROR, oss.str());

		return true;

		break;
	case vertex:
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			message = "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n\n" + std::string(infoLog);
			WriteConsoleMessage(SHADER, ERROR, message);

			return true;
		}
		break;
	case fragment:
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			message = "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n\n" + std::string(infoLog);
			WriteConsoleMessage(SHADER, ERROR, message);

			return true;
		}
		break;
	case link_shaders:
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			message = "ERROR::SHADER::PROGRAM::LINKING_FAILED\n\n" + std::string(infoLog);
			WriteConsoleMessage(SHADER, ERROR, message);

			return true;
		}
		break;
	case vertex_data_and_buffers:
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			message = "OpenGL Error: " + std::to_string(error) + "\n\n";
			WriteConsoleMessage(SHADER, ERROR, message);

			return true;
		}

		break;
	}

	return false;
}

//handles the imgui UI rendering
static void RenderUI() 
{
	//clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui::Begin("Scene view");

	//render the triangle and background inside the imgui window
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();

	//set up a framebuffer for rendering imgui content
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
	{
		//WriteConsoleMessage(SHADER, ERROR, "Framebuffer is not complete!\n\n");
	}

	//set up a viewport within the imgui window
	glViewport(0, 0, static_cast<GLsizei>(viewportSize.x), static_cast<GLsizei>(viewportSize.y));

	//clear the background to dark green
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//render the orange triangle
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//reset the framebuffer and viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//unbind and delete the framebuffer object
	glDeleteFramebuffers(1, &framebuffer);

	ImGui::End();
}

//this is run while the window is open
static void WindowLoop()
{
	processInput(window);

	//start imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//render imgui UI
	RenderUI();

	//render imgui UI on top
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	GLenum glError = glGetError();
	if (glError != GL_NO_ERROR) {
		//std::cerr << "OpenGL Error: " << glError << std::endl;
	}

	//swap the front and back buffers
	glfwSwapBuffers(window);
	//poll for events and process them
	glfwPollEvents();
}

//handles different shutdown states
static void Shutdown(MessageType reason, ErrorType errorType, const std::string& errorMessage)
{
	switch (reason)
	{
	case GLFW:
		WriteConsoleMessage(reason, errorType, errorMessage);

		glfwTerminate();
		break;
	case SHUTDOWN:
		WriteConsoleMessage(reason, errorType, errorMessage);
		WriteConsoleMessage(SHUTDOWN, INFO, "Cleaning up resources...\n");

		//de-allocate all resources once they've outlived their purpose
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteProgram(shaderProgram);

		//clean up imgui resources
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		//clean all glfw resources after program is closed
		glfwTerminate();

		WriteConsoleMessage(SHUTDOWN, SUCCESS, "Shutdown complete!\n");

		break;
	}
}

//process all input and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		WriteConsoleMessage(INPUT, SUCCESS, "User pressed ESC key to shut down engine...\n\n");

		glfwSetWindowShouldClose(window, true);
	}
}

//make sure the viewport matches the new window dimentions
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}