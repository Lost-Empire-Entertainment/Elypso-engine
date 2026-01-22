//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <array>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/key_standards.hpp"

#include "opengl/ku_opengl_shader.hpp"
#include "opengl/ku_opengl_texture.hpp"
#include "core/ku_registry.hpp"

namespace KalaUI::Core
{
	class Font; //forward-declare the font class
}
namespace KalaUI::OpenGL
{
	class OpenGL_Manager; //forward-declare the manager
}

namespace KalaUI::OpenGL::UI
{
	using std::string;
	using std::vector;
	using std::array;
	using std::function;

	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaMath::mat4;
	using KalaHeaders::KalaMath::Transform2D;
	using KalaHeaders::KalaMath::PosTarget;
	using KalaHeaders::KalaMath::RotTarget;
	using KalaHeaders::KalaMath::SizeTarget;

	using KalaHeaders::KalaKeyStandards::MouseButton;
	using KalaHeaders::KalaKeyStandards::KeyboardButton;

	using KalaUI::OpenGL::OpenGL_Shader;
	using KalaUI::OpenGL::OpenGL_Texture;
	using KalaUI::Core::KalaUIRegistry;

	constexpr u16 MAX_Z_ORDER = 1024;

	static const vector<vec2> vertices =
	{
		vec2(-0.5f,  0.5f), //top-left
		vec2(0.5f,  0.5f),  //top-right
		vec2(0.5f, -0.5f),  //bottom-right
		vec2(-0.5f, -0.5f)  //bottom-left
	};
	static const vector<u32> indices =
	{
		0, 1, 2,
		2, 3, 0
	};
	static const vector<u32> uvs =
	{
		0, 0,
		1, 0,
		1, 1,
		0, 1
	};

	enum class HitTarget
	{
		//uses the widgets own size and vertices to calculate hit testing
		HIT_QUAD,

		//uses the attached texture as the hit test,
		//defaults to quad if no texture is attached
		HIT_TEXTURE
	};

	enum class ActionTarget
	{
		ACTION_PRESSED,  //pressed key or mouse button
		ACTION_RELEASED, //released key or mouse button
		ACTION_HELD,     //held key or mouse button
		ACTION_HOVERED,  //hovered cursor over widget
		ACTION_DRAGGED,  //held mouse button and moved mouse
		ACTION_SCROLLED  //used scrollwheel
	};

	struct OpenGL_Widget_Render
	{
		bool canUpdate = true;
		
		uintptr_t glContext{};

		//no children render past this widget size if true
		bool isClipping{};

		vec3 color = vec3(1.0f);
		f32 opacity = 1.0f;

		array<vec2, 2> aabb{};

		OpenGL_Shader* shader{};
	};

	struct OpenGL_Widget_Event
	{
		function<void()> function_button_pressed{};
		KeyboardButton keyPressed{};
		MouseButton mousePressed{};

		function<void()> function_button_released{};
		KeyboardButton keyReleased{};
		MouseButton mouseReleased{};

		function<void()> function_button_held{};
		KeyboardButton keyHeld{};
		MouseButton mouseHeld{};

		function<void()> function_mouse_dragged{};
		MouseButton mouseDragged{};

		function<void()> function_mouse_hovered{};
		function<void()> function_mouse_scrolled{};
	};

	class LIB_API OpenGL_Widget
	{
		friend class KalaUI::Core::Font; //friend-include the font class
		friend class OpenGL_Manager;     //friend-include the manager
	public:
		static KalaUIRegistry<OpenGL_Widget>& GetRegistry();
	
		//Returns all hit widgets at mouse position sorted by highest Z first
		static vector<OpenGL_Widget*> GetHitWidgets(vec2 mousePos);

		//
		// CORE
		//

		bool IsInitialized() const;
		
		virtual bool Render(
			const mat4& projection,
			f32 viewportHeight) = 0;

		//Adjusts widget position relative to viewport size and offset,
		//offset at {1.0f, 1.0f} means the widget is centered, {0.0f, 0.0f} moves it to the bottom left corner
		void MoveWidget(
			vec2 viewportSize,
			vec2 offset = vec2(1.0f));

		u32 GetID() const;
		u32 GetWindowID() const;
		
		uintptr_t GetGLContext() const;

		void SetUpdateState(bool newValue);
		bool CanUpdate() const;

		//No children render past this widget size if true
		void SetClippingState(bool newValue);
		//No children render past this widget size if true
		bool IsClipping() const;

		void SetName(const string& newName);
		const string& GetName() const;

		//Should be called whenever a parent or child is added or removed from this widget
		//to ensure this widget local values are refreshed
		void ResetWidgetAfterHierarchyUpdate();

		const array<vec2, 2>& GetAABB(f32 viewportHeight);
		
		//
		// TRANSFORM
		//
		
		//Increments position over time
		void AddPos(
			PosTarget type,
			const vec2 deltaPos);
		//Snaps to given position
		void SetPos(
			PosTarget type,
			const vec2 newPos);
		vec2 GetPos(PosTarget type) const;
		
		//Increments rotation over time
		void AddRot(
			RotTarget type,
			const f32 deltaRot);
		//Snaps to given rotation
		void SetRot(
			RotTarget type,
			f32 newRot);
		f32 GetRot(RotTarget type) const;
		
		//Increments size over time
		void AddSize(
			SizeTarget type,
			const vec2 deltaSize);
		//Snaps to size position
		void SetSize(
			SizeTarget type,
			const vec2 newSize);
		vec2 GetSize(SizeTarget type) const;

		//
		// Z ORDER
		//

		//Makes this widget Z order 1 unit higher than target widget
		void MoveAbove(OpenGL_Widget* targetWidget);
		//Makes this widget Z order 1 unit lower than target widget
		void MoveBelow(OpenGL_Widget* targetWidget);

		void SetZOrder(u16 newZOrder);
		u16 GetZOrder() const;

		//
		// INTERACTION
		//

		//Skip hit testing and event polling if true
		void SetInteractableState(bool newValue);
		//Skip hit testing and event polling if true
		bool IsInteractable() const;

		//If the cursor is over this widget and this widget is not
		//covered entirely or partially by another widget then this returns true
		bool IsHovered(vec2 mousePos) const;

		//Accepts mouse buttons for pressed, released, held and dragged events.
		//Use 'SetMouseHoverEvent()' and 'SetMouseScrollEvent()' to assign those events
		void SetMouseEvent(
			const function<void()>& newValue,
			MouseButton mouseButton,
			ActionTarget actionTarget);
		//Assigns mouse hovered event
		void SetMouseHoverEvent(const function<void()>& newValue);
		//Assigns mouse scrolled event
		void SetMouseScrollEvent(const function<void()>& newValue);
		//Returns which mouse button is attached to what event, ignores hovered and scrolled events
		MouseButton GetMouseEventButton(ActionTarget actionTarget) const;

		//Accepts keyboard keys for pressed, released and held events, ignores all other events
		void SetKeyEvent(
			const function<void()>& newValue,
			KeyboardButton key,
			ActionTarget actionTarget);
		//Returns which key is attached to what key event, ignores dragged, hovered and scrolled events
		KeyboardButton GetKeyEventButton(ActionTarget actionTarget) const;

		//Clears target event function and its buttons
		void ClearEvent(ActionTarget actionTarget);
		//Removes all event functions and resets their attached buttons
		void ClearAllEvents();

		//Poll the events that have attached functions once this frame,
		//skipped internally if 'isInteractable' is false
		void PollEvents();

		//
		// GRAPHICS
		//

		void SetNormalizedColor(const vec3& newValue);
		void SetRGBColor(const vec3& newValue);

		const vec3& GetNormalizedColor() const;
		vec3 GetRGBColor() const;

		void SetOpacity(f32 newValue);
		f32 GetOpacity() const;

		const OpenGL_Shader* GetShader() const;

		//Do not destroy manually, erase from registry instead
		virtual ~OpenGL_Widget() = 0;
	protected:
		virtual void UpdateAABB(f32 viewportHeight) = 0;

		bool isInitialized{};

		string name = "NEW_WIDGET";

		u32 ID{};
		u32 windowID{};

		bool isHovered{};

		HitTarget hitTarget{};

		u16 zOrder{};

		bool isInteractable = true;

		Transform2D transform{};
		OpenGL_Widget_Render render{};
		OpenGL_Widget_Event event{};
	};
}