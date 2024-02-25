//<Elypso engine>
//    Copyright(C) < 2024 > < Lost Empire Entertainment >
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
#include "gameobject.hpp"

using Graphics::Shape::GameObject;

using glm::vec3;
using glm::mat4;

namespace Graphics::Shape
{
	class SpotLight
	{
	public:
		static inline string tempName = "123456789";
		static inline unsigned int tempID = 123456789;

		static shared_ptr<GameObject> InitializeSpotLight(
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0),
			const vec3& scale = vec3(1),
			const string& vertShader = "shaders/Basic_model.vert",
			const string& fragShader = "shaders/Basic.frag",
			const vec3& diffuse = vec3(1),
			const float& intensity = 1.0f,
			const float& distance = 1.0f,
			const float& innerAngle = 12.5f,
			const float& outerAngle = 17.5f,
			string& name = tempName,
			unsigned int& id = tempID,
			
			//
			// BILLBOARD VALUES
			//

			const string& billboardVertShader = "shaders/Basic_texture.vert",
			const string& billboardFragShader = "shaders/Basic_texture.frag",
			const string& billboardDiffTexture = "icons/spotLight.png",
			const float& billboardShininess = 32,
			string& billboardName = tempName,
			unsigned int& billboardID = tempID);

		static void RenderSpotLight(const shared_ptr<GameObject>& obj, const mat4& view, const mat4& projection);
	};
}