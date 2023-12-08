//external
#include "glad.h"
#include "glfw3.h"
#include "glm.hpp"
#include "matrix_transform.hpp"
#include "type_ptr.hpp"

//engine
#include "core.h"
#include "render.h"
#include "shader.h"
#include "texture.h"
#include "console.h"
#include "shutdown.h"
#include "search.h"

#include <string>
#include <iostream>

using namespace std;
using namespace glm;
using Caller = Core::ConsoleManager::Caller;
using Type = Core::ConsoleManager::Type;

namespace Core::Graphics
{
	Shader* Render::shader;

	void Render::RenderSetup()
	{
		Render::GLFWSetup();
		Render::WindowSetup();
		Render::GladSetup();
		Render::ContentSetup();
	}

	void Render::GLFWSetup()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::GLFW,
			Type::INFO,
			"Initializing GLFW...\n");

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		ConsoleManager::WriteConsoleMessage(
			Caller::GLFW,
			Type::SUCCESS,
			"GLFW initialized successfully!\n\n");
	}
	void Render::WindowSetup()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_SETUP,
			Type::INFO,
			"Creating window...\n");

		//create a window object holding all the windowing data
		window = glfwCreateWindow(
			SCR_WIDTH,
			SCR_HEIGHT,
			"Elypso engine",
			NULL,
			NULL);

		if (window == NULL)
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::GLFW,
				Type::ERROR,
				"Error: Failed to create GLFW window!\n\n");
			return;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, UpdateAfterRescale);

		ConsoleManager::WriteConsoleMessage(
			Caller::WINDOW_SETUP,
			Type::SUCCESS,
			"Window initialized successfully!\n\n");
	}
	void Render::GladSetup()
	{
		ConsoleManager::WriteConsoleMessage(
			Caller::GLAD,
			Type::INFO,
			"Initializing GLAD...\n");

		//check if glad is initialized before continuing
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			ConsoleManager::WriteConsoleMessage(
				Caller::GLAD,
				Type::ERROR,
				"Error: Failed to initialize GLAD!\n\n");
			return;
		}

		ConsoleManager::WriteConsoleMessage(
			Caller::GLAD,
			Type::SUCCESS,
			"GLAD initialized successfully!\n\n");
	}
	void Render::ContentSetup()
	{
		string projectPath = Core::Search::SearchByParent("Elypso engine");
		string vertexPath = projectPath + "\\src\\engine\\core\\graphics\\shaders\\vertexShader.vert";
		string fragmentPath = projectPath + "\\src\\engine\\core\\graphics\\shaders\\fragmentShader.frag";
		string texturePath = projectPath + "\\files";

		glEnable(GL_DEPTH_TEST);

		shader = new Shader(vertexPath, fragmentPath);

		float vertices[] =
		{
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};
		//cube world space positions
		cubePositions[0] = vec3(0.0f, 0.0f, 0.0f);
		cubePositions[1] = vec3(2.0f, 5.0f, -15.0f);
		cubePositions[2] = vec3(-1.5f, -2.2f, -2.5f);
		cubePositions[3] = vec3(-3.8f, -2.0f, -12.3f);
		cubePositions[4] = vec3(2.4f, -0.4f, -3.5f);
		cubePositions[5] = vec3(-1.7f, 3.0f, -7.5f);
		cubePositions[6] = vec3(1.3f, -2.0f, -2.5f);
		cubePositions[7] = vec3(1.5f, 2.0f, -2.5f);
		cubePositions[8] = vec3(1.5f, 0.2f, -1.5f);
		cubePositions[9] = vec3(-1.3f, 1.0f, -1.5f);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		Texture tex(texturePath);
		tex.LoadTexture("crate.jpg", false, GL_RGB);
		tex.LoadTexture("pepe.png", true, GL_RGBA);

		shader->Use();
		shader->SetInt("texture1", 0);
		shader->SetInt("texture2", 1);
	}

	void Render::UpdateAfterRescale(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void Render::Shutdown()
	{
		delete shader;
		shader = nullptr;
	}

	void Render::WindowLoop()
	{
		//clear the background to dark green
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//clear color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture::textures[1]);

		//activate shader
		shader->Use();

		//create transformations
		mat4 view = mat4(1.0f);
		mat4 projection = mat4(1.0f);
		view = translate(
			view, 
			vec3(0.0f, 0.0f, -3.0f));
		projection = perspective(
			radians(45.0f), 
			(float)SCR_WIDTH / (float)SCR_HEIGHT, 
			0.1f, 
			100.0f);
		//pass transformation matrices to the shader
		shader->SetMat4("view", view);
		shader->SetMat4("projection", projection);

		//render boxes
		glBindVertexArray(Render::VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			//calculate the model matrix for each object
			//and pass it to the shader before drawing
			mat4 model = mat4(1.0f);
			model = translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
			shader->SetMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//swap the front and back buffers
		glfwSwapBuffers(Render::window);
		//poll for events and process them
		glfwPollEvents();
	}
}