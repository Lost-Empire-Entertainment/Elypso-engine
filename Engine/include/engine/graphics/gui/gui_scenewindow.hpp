//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once
#if ENGINE_MODE
#include <map>
#include <string>

namespace Graphics::GUI
{
	using std::map;
	using std::string;

	class GUISceneWindow
	{
	public:
		static inline bool waitBeforeCountsUpdate;

		static inline bool isSceneSelected;
		static inline map<int, string> aspectRatio
		{
			{0, "Unlocked"},
			{1, "16:9"},
			{2, "16:10"},
			{3, "21:9"},
			{4, "32:9"},
			{5, "4:3"},
		};
		static inline int currentIndex = 0;

		static void RenderSceneWindow();
		static void UpdateCounts();
	private:
		static inline bool showSceneWindowDebugMenu = true;
		static inline bool showLeftCornerContent = true;
		static inline int objectsCount = 0;
		static inline int verticesCount = 0;

		static void RenderSceneWindowMainContent();
		static void RenderSceneWindowLeftContent();
		static void RenderSceneWindowRightContent();
		static void RenderSceneWindowActionButtons();
	};
}
#endif