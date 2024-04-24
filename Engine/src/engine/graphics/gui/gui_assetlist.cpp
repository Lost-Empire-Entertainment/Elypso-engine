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

using Graphics::Shape::GameObjectManager;

namespace Graphics::GUI
{
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

			ImVec2 categoriesChildSize = ImVec2(
				280,
				ImGui::GetWindowSize().y - 100);
			ImGui::BeginChild(
				"##categories", 
				categoriesChildSize, 
				true);
			DrawCategoriesHierarchy();
			ImGui::EndChild();
			
			ImGui::SetCursorPos(ImVec2(300, 70));
			ImVec2 gameobjectsChildSize = ImVec2(
				ImGui::GetWindowSize().x - 310,
				ImGui::GetWindowSize().y - 100);
			ImGui::BeginChild(
				"##gameobjects", 
				gameobjectsChildSize, 
				true,
				ImGuiWindowFlags_HorizontalScrollbar);
			DrawGameobjectsTable();
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

					if (ImGui::IsItemClicked(0)
						&& ImGui::IsMouseDoubleClicked(0))
					{
						cout << "opening " << subCategoryName << "\n";
					}
				}

				ImGui::TreePop();
			}
		}
	}

	void GUIAssetList::DrawGameobjectsTable()
	{
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		static float horizontalStartX = 10.0f;
		static float verticalStartY = 10.0f;
		static float horizontalSpace = 100;
		static float verticalSpace = 50;

		static int rowCount = 15;
		static int columnCount = 5;
		static float lineThickness = 1.0f;

		float tablePosX = windowPos.x + horizontalStartX;
		float tablePosY = windowPos.y + verticalStartY;

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
	}
}