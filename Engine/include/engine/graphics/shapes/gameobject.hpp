//<Elypso engine>
//    Copyright(C) < 2024 > < Greenlaser >
//
//    This program is free software : you can redistribute it and /or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License in LICENCE.md
//    and a copy of the EULA in EULA.md along with this program. 
//    If not, see < https://github.com/Lost-Empire-Entertainment/Elypso-engine >.

#pragma once

//external
#include "glm.hpp"

//engine
#include "shader.hpp"

#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;
using std::make_unique;
using glm::vec3;

using Graphics::Shader;

namespace Graphics::Shape
{
	enum class MeshType
	{
		cube,
		sphere,
		cylinder,
		cone,
		pyramid,
		assimp,
		point_light,
		spot_light,
		directional_light
	};

	struct Transform
	{
		vec3 position;
		vec3 rotation;
		vec3 scale;
	};

	struct Mesh
	{
		MeshType type;
	};

	struct Material
	{
		Shader shader;
		GLuint VAO;
		GLuint VBO;
	};

	struct GameObject
	{
		string name;
		unsigned int ID;

		//components
		Transform transform;
		Mesh mesh;
		Material material;
	};

	class GameObjectManager
	{
	public:
		GameObject* CreateObject(
			const string& name, 
			const unsigned int& ID);
		void DestroyObject(GameObject* obj);

		vector<shared_ptr<GameObject>> objects;
		vector<shared_ptr<GameObject>> pointLights;
	};
}