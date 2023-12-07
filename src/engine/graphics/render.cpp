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

namespace Graphics
{
	Shader* Render::shader;
	bool Render::shaderInitialized = false;

	int Render::WindowSetup()
	{
		std::string projectPath = Core::Search::SearchByParent("Elypso engine");
		std::string vertexPath = projectPath + "\\src\\engine\\graphics\\shaders\\vertexShader.vert";
		std::string fragmentPath = projectPath + "\\src\\engine\\graphics\\shaders\\fragmentShader.frag";
		std::string texturePath = projectPath + "\\files";

		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::GLFW,
			Core::Console::ConsoleManager::Type::INFO,
			"Initializing GLFW...\n");
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::GLFW,
			Core::Console::ConsoleManager::Type::SUCCESS,
			"GLFW initialized successfully!\n\n");

		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::WINDOW_SETUP,
			Core::Console::ConsoleManager::Type::INFO,
			"Creating window...\n");
		//create a window object holding all the windowing data
		Render::window = glfwCreateWindow(
			Render::SCR_WIDTH,
			Render::SCR_HEIGHT,
			"Elypso engine",
			NULL,
			NULL);
		if (Render::window == NULL)
		{
			Core::Console::ConsoleManager::WriteConsoleMessage(
				Core::Console::ConsoleManager::Caller::GLFW,
				Core::Console::ConsoleManager::Type::ERROR,
				"Error: Failed to create GLFW window!\n\n");
			Core::ShutdownManager::Shutdown();
			return -1;
		}
		glfwMakeContextCurrent(Render::window);
		glfwSetFramebufferSizeCallback(
			Render::window,
			Render::UpdateAfterRescale);
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::WINDOW_SETUP,
			Core::Console::ConsoleManager::Type::SUCCESS,
			"Window initialized successfully!\n\n");

		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::GLAD,
			Core::Console::ConsoleManager::Type::INFO,
			"Initializing GLAD...\n");
		//check if glad is initialized before continuing
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Core::Console::ConsoleManager::WriteConsoleMessage(
				Core::Console::ConsoleManager::Caller::GLAD,
				Core::Console::ConsoleManager::Type::ERROR,
				"Error: Failed to initialize GLAD!\n\n");
			return -1;
		}
		Core::Console::ConsoleManager::WriteConsoleMessage(
			Core::Console::ConsoleManager::Caller::GLAD,
			Core::Console::ConsoleManager::Type::SUCCESS,
			"GLAD initialized successfully!\n\n");

		glEnable(GL_DEPTH_TEST);

		if (!shaderInitialized)
		{
			shader = new Shader(vertexPath, fragmentPath);

			shaderInitialized = true;
		}

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
		unsigned int indices[] =
		{
			0, 1, 3, //first triangle
			1, 2, 3  //second triangle
		};

		glGenVertexArrays(1, &Render::VAO);
		glGenBuffers(1, &Render::VBO);
		glGenBuffers(1, &Render::EBO);

		glBindVertexArray(Render::VAO);

		glBindBuffer(GL_ARRAY_BUFFER, Render::VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Render::EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

		return 0;
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
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::rotate(
			model, 
			(float)glfwGetTime(),
			glm::vec3(0.5f, 1.0f, 0.0f));
		view = glm::translate(
			view, 
			glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(
			glm::radians(45.0f), 
			(float)Render::SCR_WIDTH / (float)Render::SCR_HEIGHT, 
			0.1f, 
			100.0f);
		//retrieve the matrix uniform locations
		unsigned int modelLoc = glGetUniformLocation(shader->ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader->ID, "view");
		//pass them to the shaders
		shader->SetMat4("model", model);
		shader->SetMat4("view", view);
		shader->SetMat4("projection", projection);

		//render container
		glBindVertexArray(Render::VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//swap the front and back buffers
		glfwSwapBuffers(Render::window);
		//poll for events and process them
		glfwPollEvents();
	}
}