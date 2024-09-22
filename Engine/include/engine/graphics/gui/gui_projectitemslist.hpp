//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <memory>

//engine
#include "gameobject.hpp"

using std::string;
using std::vector;
using std::shared_ptr;

using Graphics::Shape::Material;
using Graphics::Shape::GameObject;

namespace Graphics::GUI
{
	class GUIProjectItemsList
	{
	public:
		enum class Type
		{
			SkyboxTexture_right,
			SkyboxTexture_left,
			SkyboxTexture_top,
			SkyboxTexture_bottom,
			SkyboxTexture_front,
			SkyboxTexture_back,
			GameobjectTexture,
			Scene
		};

		static inline bool renderProjectItemsList;
		static inline Type type;
		static inline string selectedPath;

		//for assigning scene

		static inline bool selectStartScene;

		//for assigning texture to skybox

		static inline bool selectSkyboxTexture;
		
		//for assigning texture to gameobjects
		
		static inline Material::TextureType textureType;
		static inline shared_ptr<GameObject> obj;

		static void RenderProjectItemsList();
	private:
		static inline bool isContentVectorFilled;
		static inline vector<string> content;

		static void RenderProjectItemsListContent();
	};
}