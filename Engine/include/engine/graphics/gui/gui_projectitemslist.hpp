//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

//engine
#include "gameobject.hpp"
#include "materialcomponent.hpp"

namespace Graphics::GUI
{
	using std::string;
	using std::vector;
	using std::shared_ptr;
	using std::unordered_map;

	using Graphics::Components::MaterialComponent;
	using Graphics::Shape::GameObject;

	class GUIProjectItemsList
	{
	public:
		enum class Type
		{
			SkyboxTexture_right,  //The skybox texture in the right
			SkyboxTexture_left,   //The skybox texture in the left
			SkyboxTexture_top,    //The skybox texture at the top
			SkyboxTexture_bottom, //The skybox texture at the bottom
			SkyboxTexture_front,  //The skybox texture in the front
			SkyboxTexture_back,   //The skybox texture at the back
			GameobjectTexture,    //The texture attached to the gameobject
			GameobjectModel,      //The model file used by the gameobject
			Scene,                //The chosen scene file
			Audio                 //The chosen audio file
		};

		static inline bool renderProjectItemsList;
		static inline Type type;
		static inline string selectedPath;

		//for assigning scene

		static inline bool selectStartScene;

		//for assigning texture to skybox

		static inline bool selectSkyboxTexture;
		
		//for assigning texture to gameobjects
		
		static inline MaterialComponent::TextureType textureType;
		static inline shared_ptr<GameObject> obj;

		static void RenderProjectItemsList();
	private:
		static inline bool renderLargeImportConfirm;
		static inline bool isContentVectorFilled;
		static inline vector<string> content;

		static inline string name;
		static inline double size;

		static void RenderProjectItemsListContent();

		static void RenderLargeInitializeConfirm();
		static void ModelImportCheck();
		static void Initialize();
	};
}
#endif