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

#include "gameobject.hpp"
#include "cube.hpp"
#include "pointlight.hpp"

#include <iostream>

using std::cout;
using std::endl;
using std::to_string;
using std::remove;
using std::dynamic_pointer_cast;

using Type = Graphics::Shape::Mesh::MeshType;

namespace Graphics::Shape
{
	//
	// TRANSFORM
	//

	Transform::Transform(
		const vec3& assignedPosition,
		const vec3& assignedRotation,
		const vec3& assignedScale) :
		position(assignedPosition),
		rotation(assignedRotation),
		scale(assignedScale)
	{
		SetPosition(assignedPosition);
		SetRotation(assignedRotation);
		SetScale(assignedScale);
	}

	//
	// MESH
	//

	Mesh::Mesh(const MeshType& assignedType) : type(assignedType)
	{
		SetMeshType(assignedType);
	}

	//
	// MATERIAL
	//

	Material::Material(
		const Shader& assignedShader,
		const GLuint& assignedVAO,
		const GLuint& assignedVBO) :
		shader(assignedShader),
		VAO(assignedVAO),
		VBO(assignedVBO)
	{
		SetShader(assignedShader);
		SetVAO(assignedVAO);
		SetVBO(assignedVBO);
	}

	//
	// MATERIAL VALUES
	//

	BasicShape_Variables::BasicShape_Variables(const float& assignedShininess) : shininess(assignedShininess)
	{
		SetShininess(assignedShininess);
	}

	SpotLight_Variables::SpotLight_Variables(
		const vec3& assignedDiffuse,
		const float& assignedIntensity,
		const float& assignedDistance,
		const float& assignedInnerAngle,
		const float& assignedOuterAngle) :
		diffuse(assignedDiffuse),
		intensity(assignedIntensity),
		distance(assignedDistance),
		innerAngle(assignedInnerAngle),
		outerAngle(assignedOuterAngle)
	{
		SetDiffuse(assignedDiffuse);
		SetIntensity(assignedIntensity);
		SetDistance(assignedDistance);
		SetInnerAngle(assignedInnerAngle);
		SetOuterAngle(assignedOuterAngle);
	}

	PointLight_Variables::PointLight_Variables(
		const vec3& assignedDiffuse,
		const float& assignedIntensity,
		const float& assignedDistance) :
		diffuse(assignedDiffuse),
		intensity(assignedIntensity),
		distance(assignedDistance)
	{
		SetDiffuse(assignedDiffuse);
		SetIntensity(assignedIntensity);
		SetDistance(assignedDistance);
	}

	//
	// GAMEOBJECT
	//

	GameObject::GameObject(
		const bool& assignedIsInitialized,
		const string& assignedName, 
		const unsigned int& assignedID, 
		const shared_ptr<Transform>& assignedTransform,
		const shared_ptr<Mesh>& assignedMesh, 
		const shared_ptr<Material>& assignedMaterial,
		const shared_ptr<BasicShape_Variables>& assignedBasicShape) :
		isInitialized(assignedIsInitialized),
		name(assignedName),
		ID(assignedID),
		transform(assignedTransform),
		mesh(assignedMesh),
		material(assignedMaterial),
		basicShape(assignedBasicShape)
	{
		SetName(assignedName);
		SetID(nextID++);
		SetTransform(assignedTransform);
		SetMesh(assignedMesh);
		SetMaterial(assignedMaterial);
		SetBasicShape(assignedBasicShape);

		Initialize();
	}

	GameObject::GameObject(
		const bool& assignedIsInitialized,
		const string& assignedName,
		const unsigned int& assignedID,
		const shared_ptr<Transform>& assignedTransform,
		const shared_ptr<Mesh>& assignedMesh,
		const shared_ptr<Material>& assignedMaterial,
		const shared_ptr<PointLight_Variables>& assignedPointLight) :
		isInitialized(assignedIsInitialized),
		name(assignedName),
		ID(assignedID),
		transform(assignedTransform),
		mesh(assignedMesh),
		material(assignedMaterial),
		pointLight(assignedPointLight)
	{
		SetName(assignedName);
		SetID(nextID++);
		SetTransform(assignedTransform);
		SetMesh(assignedMesh);
		SetMaterial(assignedMaterial);
		SetPointLight(assignedPointLight);

		Initialize();
	}

	GameObject::GameObject(
		const bool& assignedIsInitialized,
		const string& assignedName,
		const unsigned int& assignedID,
		const shared_ptr<Transform>& assignedTransform,
		const shared_ptr<Mesh>& assignedMesh,
		const shared_ptr<Material>& assignedMaterial,
		const shared_ptr<SpotLight_Variables>& assignedSpotLight) :
		isInitialized(assignedIsInitialized),
		name(assignedName),
		ID(assignedID),
		transform(assignedTransform),
		mesh(assignedMesh),
		material(assignedMaterial),
		spotLight(assignedSpotLight)
	{
		SetName(assignedName);
		SetID(nextID++);
		SetTransform(assignedTransform);
		SetMesh(assignedMesh);
		SetMaterial(assignedMaterial);
		SetSpotLight(assignedSpotLight);

		Initialize();
	}

	//
	// GAMEOBJECT MANAGER
	//

	void GameObjectManager::RenderAll(const mat4& view, const mat4& projection)
	{
		if (objects.size() > 0)
		{
			for (const auto& obj : objects)
			{
				Type type = obj->GetMesh()->GetMeshType();
				switch (type)
				{
				case Type::cube:
					Cube::RenderCube(obj, view, projection);
					break;
				case Type::point_light:
					PointLight::RenderPointLight(obj, view, projection);
					break;
				}
			}
		}
	}

	void GameObjectManager::AddGameObject(const shared_ptr<GameObject>& obj)
	{
		objects.push_back(obj);
	}

	void GameObjectManager::AddPointLight(const shared_ptr<GameObject>& obj)
	{
		pointLights.push_back(obj);
	}

	void GameObjectManager::AddSpotLight(const shared_ptr<GameObject>& obj)
	{
		spotLights.push_back(obj);
	}

	void GameObjectManager::DestroyGameObject(const shared_ptr<GameObject>& obj)
	{
		auto it = remove(objects.begin(), objects.end(), obj);
		objects.erase(it, objects.end());
	}
}