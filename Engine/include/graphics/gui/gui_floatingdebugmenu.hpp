//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

namespace Graphics::GUI
{
	class GUIFloatingDebugMenu
	{
	public:
		static inline bool waitBeforeUpdate;

		static void RenderFloatingDebugMenu();

		/// <summary>
		/// Updates floating debug menu vertice and object counts
		/// </summary>
		static void UpdateCounts();
	private:
		static inline int objectsCount = 0;
		static inline int verticesCount = 0;

		static void FloatingDebugMenuContent();
	};
}