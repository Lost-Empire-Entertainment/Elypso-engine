//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>

//engine
#include "gameobject.hpp"

namespace Graphics::Components
{
	using std::vector;

	using Graphics::Shape::Component;
	using Graphics::Shape::AssimpVertex;

	class Mesh : public Component
	{
	public:
		enum class MeshType
		{
			border,
			actionTex,
			model,
			point_light,
			spot_light,
			directional_light,
			billboard,
			skybox
		};

		Mesh(const bool& isEnabled,
			const MeshType& type) :
			isEnabled(isEnabled),
			type(type),
			VAO(0),
			VBO(0),
			EBO(0) {
		}
		~Mesh()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}

		void Initialize(const MeshType& meshType, const float* vertices, size_t vertexSize);
		void InitializeModel(const MeshType& meshType, const vector<AssimpVertex> assimpVertex);

		void SetEnableState(const bool& newIsEnabled)
		{
			isEnabled = newIsEnabled;
		}
		void SetMeshType(const MeshType& newType)
		{
			type = newType;
		}
		void SetVAO(const GLuint& newVAO)
		{
			VAO = newVAO;
		}
		void SetVBO(const GLuint& newVBO)
		{
			VBO = newVBO;
		}
		void SetEBO(const GLuint& newEBO)
		{
			EBO = newEBO;
		}
		void SetVertices(const vector<AssimpVertex>& newVertices)
		{
			vertices = newVertices;
		}
		void SetIndices(const vector<unsigned int>& newIndices)
		{
			indices = newIndices;
		}

		const bool& IsEnabled() const
		{
			return isEnabled;
		}
		const MeshType& GetMeshType() const
		{
			return type;
		}
		const GLuint& GetVAO() const
		{
			return VAO;
		}
		const GLuint& GetVBO() const
		{
			return VBO;
		}
		const GLuint& GetEBO() const
		{
			return EBO;
		}
		const vector<AssimpVertex>& GetVertices() const
		{
			return vertices;
		}
		const vector<unsigned int>& GetIndices() const
		{
			return indices;
		}
	private:
		bool isEnabled;
		MeshType type;
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		vector<AssimpVertex> vertices;
		vector<unsigned int> indices;
	};
}