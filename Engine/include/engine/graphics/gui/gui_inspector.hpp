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

namespace Graphics::GUI
{
	class GUIInspector
	{
	public:
		static inline bool renderInspector;
		static void RenderInspector();
	private:
		static inline const int bufferSize = 32;
		static inline char inputTextBuffer_pos_x[bufferSize];
		static inline char inputTextBuffer_pos_y[bufferSize];
		static inline char inputTextBuffer_pos_z[bufferSize];
		static inline char inputTextBuffer_rot_x[bufferSize];
		static inline char inputTextBuffer_rot_y[bufferSize];
		static inline char inputTextBuffer_rot_z[bufferSize];
		static inline char inputTextBuffer_scale_x[bufferSize];
		static inline char inputTextBuffer_scale_y[bufferSize];
		static inline char inputTextBuffer_scale_z[bufferSize];

		static void RSM_SelectedObject();
		static void RSM_Main();
		static void RSM_Spot();
		static void RSM_Point();
		static void RSM_Cube();
	};
}