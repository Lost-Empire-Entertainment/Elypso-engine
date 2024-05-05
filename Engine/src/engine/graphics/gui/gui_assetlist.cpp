//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//external
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//engine
#include "gui_assetlist.hpp"
#include "gui.hpp"
#include "gameobject.hpp"
#include "selectobject.hpp"
#include "stringUtils.hpp"

using std::exception;

using Graphics::Shape::GameObjectManager;
using Graphics::Shape::GameObject;
using Graphics::Shape::Mesh;
using Physics::Select;
using Utils::String;

namespace Graphics::GUI
{
	GUIAssetList assetList;
	static shared_ptr<GameObject> selectedObj;
	static GameObject::Category chosenCategory = GameObject::Category::cat_All;

	void GUIAssetList::RenderAssetList()
	{
		ImGui::SetNextWindowSizeConstraints(EngineGUI::minSize, EngineGUI::maxSize);
		ImGui::SetNextWindowPos(EngineGUI::initialPos, ImGuiCond_FirstUseEver);

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse;

		if (renderAssetList
			&& ImGui::Begin("Asset list", NULL, windowFlags))
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
			if (ImGui::Button("X"))
			{
				renderAssetList = false;
			}

			//draw categories hierarchy
			ImVec2 categoriesChildSize = ImVec2(
				280,
				ImGui::GetWindowSize().y - 100);
			ImGui::BeginChild(
				"##categories", 
				categoriesChildSize, 
				true);
			assetList.DrawCategoriesHierarchy();
			ImGui::EndChild();
			
			//draw gameobject table
			ImGui::SetCursorPos(ImVec2(300, 70));
			ImVec2 gameobjectsChildSize = ImVec2(
				ImGui::GetWindowSize().x - 310,
				ImGui::GetWindowSize().y - 100);
			ImGui::BeginChild(
				"##gameobjects", 
				gameobjectsChildSize, 
				true,
				ImGuiWindowFlags_HorizontalScrollbar);
			assetList.DrawGameobjectsTable();
			ImGui::EndChild();

			ImGui::End();
		}
	}

	void GUIAssetList::DrawCategoriesHierarchy()
	{
		for (const auto& category : GameObjectManager::GetCategoryNames())
		{
			const string& categoryName = category.first;
			const auto& categoryMap = category.second;

			bool nodeOpen = ImGui::TreeNodeEx(categoryName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

			if (ImGui::IsItemClicked(0)
				&& ImGui::IsMouseDoubleClicked(0))
			{
				nodeOpen = !nodeOpen;
			}

			if (nodeOpen)
			{
				for (const auto& subCategory : categoryMap)
				{
					const string& subCategoryName = subCategory;
					ImGui::Text("%s", subCategoryName.c_str());

					//double-click a category to list its gameobjects in table
					if (ImGui::IsItemClicked(0)
						&& ImGui::IsMouseDoubleClicked(0))
					{
						if (categoryName == "Characters"
							&& subCategoryName == "Placeholder")
						{
							chosenCategory = GameObject::Category::cat_Characters_Placeholder;
						}

						else if (categoryName == "Effects"
							     && subCategoryName == "Placeholder")
						{
							chosenCategory = GameObject::Category::cat_Effects_Placeholder;
						}

						else if (categoryName == "Audio"
							     && subCategoryName == "Placeholder")
						{
							chosenCategory = GameObject::Category::cat_Audio_Placeholder;
						}

						else if (categoryName == "UI"
							     && subCategoryName == "Placeholder")
						{
							chosenCategory = GameObject::Category::cat_UI_Placeholder;
						}

						else if (categoryName == "Lights")
						{
							if (subCategoryName == "Spotlights")
							{
								chosenCategory = GameObject::Category::cat_Lights_Spotlights;
							}
							else if (subCategoryName == "Point lights")
							{
								chosenCategory = GameObject::Category::cat_Lights_Point_lights;
							}
						}

						else if (categoryName == "Textures")
						{
							if (subCategoryName == "Diffuse textures")
							{
								chosenCategory = GameObject::Category::cat_Textures_Diffuse_textures;
							}
							else if (subCategoryName == "Specular textures")
							{
								chosenCategory = GameObject::Category::cat_Textures_Specular_textures;
							}
							else if (subCategoryName == "Normal textures")
							{
								chosenCategory = GameObject::Category::cat_Textures_Normal_textures;
							}
							else if (subCategoryName == "Height textures")
							{
								chosenCategory = GameObject::Category::cat_Textures_Height_textures;
							}
						}

						else if (categoryName == "Props"
							     && subCategoryName == "Static props")
						{
							chosenCategory = GameObject::Category::cat_Props_Static_props;
						}
					}
				}

				ImGui::TreePop();
			}
		}

		string all = "          All";
		ImGui::Text("%s", all.c_str());

		//double-click All to list all gameobjects in project
		if (ImGui::IsItemClicked(0)
			&& ImGui::IsMouseDoubleClicked(0))
		{
			chosenCategory = GameObject::Category::cat_All;
		}
	}

	void GUIAssetList::DrawGameobjectsTable()
	{
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		//build asset list grid
		static float horizontalStartX = 10.0f;
		static float verticalStartY = 50.0f;
		static float horizontalSpace = 100;
		static float verticalSpace = 50;

		vector<shared_ptr<GameObject>> objects = GameObjectManager::GetObjects();
		vector<shared_ptr<GameObject>> displayedObjects;

		string fullName = string(magic_enum::enum_name(chosenCategory));
		fullName = String::StringReplace(fullName, "cat_", "");

		size_t pos = fullName.find('_');

		//split the string at the position of the first underscore
		string categoryName = fullName.substr(0, pos);
		string subCategoryName = fullName.substr(pos + 1);
		subCategoryName = String::StringReplace(subCategoryName, "_", " ");
		string displayedCategoryName = categoryName == "All" ? 
			"All" 
			: categoryName + " - " + subCategoryName;

		ImGui::Text(displayedCategoryName.c_str());

		//check if original has GameObjects valid doesn't
		for (const auto& obj : objects)
		{
			if (find(
				displayedObjects.begin(), 
				displayedObjects.end(), 
				obj) == displayedObjects.end()
				&& obj->GetCategoryState(chosenCategory))
			{
				displayedObjects.push_back(obj);
			}
		}

		//check if valid has GameObjects original doesn't have and remove them
		auto it = displayedObjects.begin();
		while (it != displayedObjects.end())
		{
			if (find(
				objects.begin(), 
				objects.end(), 
				*it) == objects.end()
				|| !(*it)->GetCategoryState(chosenCategory))
			{
				it = displayedObjects.erase(it);
			}
			else
			{
				++it;
			}
		}

		int rowCount = static_cast<int>(displayedObjects.size());
		if (objects.size() < 2)
		{
			rowCount = 2;
			if (displayedObjects.size() == 0)
			{
				ImGui::SetCursorPos(ImVec2(20, 60 + verticalStartY));
				ImGui::SetNextItemWidth(75);
				ImGui::Text("None");
			}
		}
		else rowCount = static_cast<int>(displayedObjects.size()) + 1;

		static int columnCount = 2;
		static float lineThickness = 1.0f;

		float tablePosX = windowPos.x + horizontalStartX;
		float tablePosY = windowPos.y + verticalStartY;

		//draw horizontal lines
		for (int row = 0; row <= rowCount; ++row)
		{
			ImVec2 horizontalStart(
				tablePosX,
				tablePosY + row * verticalSpace);
			ImVec2 horizontalEnd(
				tablePosX + columnCount * horizontalSpace,
				tablePosY + row * verticalSpace);

			ImGui::GetWindowDrawList()->AddLine(
				horizontalStart,
				horizontalEnd,
				IM_COL32(255, 255, 255, 255),
				lineThickness);
		}

		//draw vertical lines
		for (int col = 0; col <= columnCount; ++col)
		{
			ImVec2 verticalStart(
				tablePosX + col * horizontalSpace,
				tablePosY);
			ImVec2 verticalEnd(
				tablePosX + col * horizontalSpace,
				tablePosY + rowCount * verticalSpace);

			ImGui::GetWindowDrawList()->AddLine(
				verticalStart,
				verticalEnd,
				IM_COL32(255, 255, 255, 255),
				lineThickness);
		}

		string column_Name = "Name";
		ImGui::SetCursorPos(ImVec2(40, 15 + verticalStartY));
		ImGui::Text(column_Name.c_str());

		string column_ID = "ID";
		ImGui::SetCursorPos(ImVec2(150, 15 + verticalStartY));
		ImGui::Text(column_ID.c_str());

		//rows for gameobject text fields
		for (int i = 0; i < displayedObjects.size(); i++)
		{
			selectedObj = displayedObjects[i];
			int index = i;

			float cursorHeight = static_cast<float>(60 + (50 * i + 1));

			ImGui::SetCursorPos(ImVec2(20, cursorHeight + verticalStartY));
			ImGui::SetNextItemWidth(75);
			//change selected text color to yellow, otherwise white
			if (Select::selectedObj == selectedObj)
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", selectedObj->GetName().c_str());
			}
			else
			{
				ImGui::Text(selectedObj->GetName().c_str());
			}

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				Select::selectedObj = selectedObj;
				Select::isObjectSelected = true;
			}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				Select::selectedObj = selectedObj;
				Select::isObjectSelected = true;

				ImGui::OpenPopup("popUp_AssetList");
			}

			ImGui::SetCursorPos(ImVec2(120, cursorHeight + verticalStartY));
			ImGui::SetNextItemWidth(75);
			ImGui::Text(to_string(selectedObj->GetID()).c_str());
		}

		if (ImGui::BeginPopup("popUp_AssetList"))
		{
			if (ImGui::MenuItem("Delete"))
			{
				GameObjectManager::DestroyGameObject(Select::selectedObj);
			}

			ImGui::EndPopup();
		}
	}
}