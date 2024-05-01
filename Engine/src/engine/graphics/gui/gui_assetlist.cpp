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

using std::exception;

using Graphics::Shape::GameObjectManager;
using Graphics::Shape::GameObject;
using Graphics::Shape::Mesh;
using Physics::Select;

namespace Graphics::GUI
{
	GUIAssetList assetList;
	static shared_ptr<GameObject> selectedObj;

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
		for (const auto& category : GameObjectManager::GetGameObjectCategories())
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
						cout << "opening " << subCategoryName << "\n";
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
			cout << "opening All\n";
		}
	}

	void GUIAssetList::DrawGameobjectsTable()
	{
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		vector<shared_ptr<GameObject>> models = GameObjectManager::GetModels();

		//build asset list grid
		static float horizontalStartX = 10.0f;
		static float verticalStartY = 10.0f;
		static float horizontalSpace = 100;
		static float verticalSpace = 50;

		int rowCount = static_cast<int>(models.size());
		if (models.size() < 2)
		{
			rowCount = 2;
			if (models.size() == 0)
			{
				ImGui::SetCursorPos(ImVec2(20, 70));
				ImGui::SetNextItemWidth(75);
				ImGui::Text("None");
			}
		}
		else rowCount = static_cast<int>(models.size()) + 1;

		static int columnCount = 1; //we only need one column for the object name
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

		//draw object name text in main column
		string column_Name = "Name";
		ImGui::SetCursorPos(ImVec2(40, 25));
		ImGui::Text(column_Name.c_str());

		//rows for gameobject text fields
		for (int i = 0; i < models.size(); i++)
		{
			float cursorHeight = static_cast<float>(70 + (50 * i + 1));

			selectedObj = models[i];
			ImGui::SetCursorPos(ImVec2(20, cursorHeight));
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