//Copyright(C) 2025 Lost Empire Entertainment
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

#include "opengl/kg_opengl_shader.hpp"
#include "opengl/kg_opengl_texture.hpp"
#include "core/kg_registry.hpp"

namespace KalaGraphics::OpenGL::UI
{
	using std::string;
	using std::vector;
	using std::array;
	using std::function;

	using KalaHeaders::vec2;
	using KalaHeaders::vec3;
	using KalaHeaders::mat4;
	using KalaHeaders::kclamp;
	using KalaHeaders::setpos;
	using KalaHeaders::setrot;
	using KalaHeaders::setsize;
	using KalaHeaders::wrap;
	using KalaHeaders::Transform2D;
	using KalaHeaders::PosTarget;
	using KalaHeaders::RotTarget;
	using KalaHeaders::SizeTarget;
	using KalaHeaders::KalaKeyStandards::MouseButton;
	using KalaHeaders::KalaKeyStandards::KeyboardButton;

	using KalaGraphics::OpenGL::OpenGL_Shader;
	using KalaGraphics::OpenGL::OpenGL_Texture;
	using KalaGraphics::Core::KalaGraphicsRegistry;

	constexpr u16 MAX_Z_ORDER = 1024;

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

		//no children render past this widget size if true
		bool isClipping{};

		vec3 color = vec3(1.0f);
		f32 opacity = 1.0f;

		u32 VAO{};
		u32 VBO{};
		u32 EBO{};

		vector<vec2> vertices =
		{
			vec2(-0.5f,  0.5f), //top-left
			vec2(0.5f,  0.5f),  //top-right
			vec2(0.5f, -0.5f),  //bottom-right
			vec2(-0.5f, -0.5f)  //bottom-left
		};
		vector<u32> indices =
		{
			0, 1, 2,
			2, 3, 0
		};

		array<vec2, 2> aabb{};

		OpenGL_Shader* shader{};
		OpenGL_Texture* texture{};
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
	public:
		static inline KalaGraphicsRegistry<OpenGL_Widget> registry{};
	
		//Returns all hit widgets at mouse position sorted by highest Z first
		static vector<OpenGL_Widget*> GetHitWidgets(vec2 mousePos);

		//
		// CORE
		//

		inline bool IsInitialized() const { return isInitialized; }

		//Render this widget. Requires handle (HDC) from your window
		virtual bool Render(
			uintptr_t handle,
			const mat4& projection,
			f32 viewportHeight) = 0;

		//Adjusts widget position relative to viewport size and offset,
		//offset at {1.0f, 1.0f} means the widget is centered, {0.0f, 0.0f} moves it to the bottom left corner
		inline void MoveWidget(
			vec2 viewportSize,
			vec2 offset = vec2(1.0f))
		{
			if (viewportSize <= vec2(1.0f)) return;

			vec2 offsetClamped = kclamp(
				offset, 
				vec2(-0.5f), 
				vec2(2.5f));

			setpos(
				transform,
				{},
				PosTarget::POS_WORLD,
				vec2(viewportSize * offsetClamped * 0.5f));
		}

		inline u32 GetID() const { return ID; }
		inline u32 GetWindowID() const { return windowID; }
		inline u32 GetGLID() const { return glID; }

		//Skips rendering if set to false without needing to
		//encapsulate the render function in its own render toggle
		inline void SetUpdateState(bool newValue) { render.canUpdate = newValue; }
		//Skips rendering if set to false without needing to
		//encapsulate the render function in its own render toggle
		inline bool CanUpdate() const { return render.canUpdate; }

		//No children render past this widget size if true
		inline void SetClippingState(bool newValue) { render.isClipping = newValue; }
		//No children render past this widget size if true
		inline bool IsClipping() const { return render.isClipping; }

		inline void SetName(const string& newName)
		{
			//skip if name is empty, same as existing or too long
			if (newName.empty()
				|| newName == name
				|| newName.length() > 50) return;

			name = newName;
		}
		inline const string& GetName() const { return name; }

		//Should be called whenever a parent or child is added or removed from this widget
		//to ensure this widget local values are refreshed
		inline void ResetWidgetAfterHierarchyUpdate()
		{
			setpos(transform, {}, PosTarget::POS_LOCAL, vec2(0.0f));
			setrot(transform, {}, RotTarget::ROT_LOCAL, 0.0f);
			setsize(transform, {}, SizeTarget::SIZE_LOCAL, 1.0f);
		}
		
		inline void SetVertices(const vector<vec2>& newVertices) { render.vertices = newVertices; }
		inline void SetIndices(const vector<u32>& newIndices) { render.indices = newIndices; }

		inline const vector<vec2>& GetVertices() const { return render.vertices; }
		inline const vector<u32>& GetIndices() const { return render.indices; }

		inline const array<vec2, 2>& GetAABB(f32 viewportHeight)
		{ 
			UpdateAABB(viewportHeight);
			return render.aabb; 
		}
		
		//
		// TRANSFORM
		//
		
		//Increments position over time
		inline void AddPos(
			PosTarget type,
			const vec2 deltaPos)
		{
			addpos(
				transform,
				{},
				type,
				deltaPos);
		}
		//Snaps to given position
		inline void SetPos(
			PosTarget type,
			const vec2 newPos)
		{
			setpos(
				transform,
				{},
				type,
				newPos);
		}
		inline vec2 GetPos(PosTarget type) 
		{ 
			return getpos(
				transform,
				type); 
		}
		
		//Increments rotation over time
		inline void AddRot(
			RotTarget type,
			const f32 deltaRot)
		{
			f32 safeRot = wrap(deltaRot);
			
			addrot(
				transform,
				{},
				type,
				safeRot);
		}
		//Snaps to given rotation
		inline void SetRot(
			RotTarget type,
			f32 newRot)
		{
			f32 safeRot = wrap(newRot);
			
			setrot(
				transform,
				{},
				type,
				safeRot);
		}
		inline f32 GetRot(RotTarget type) 
		{ 
			return getrot(
				transform,
				type);
		}
		
		//Increments size over time
		inline void AddSize(
			SizeTarget type,
			const vec2 deltaSize)
		{
			addsize(
				transform,
				{},
				type,
				deltaSize);
		}
		//Snaps to size position
		inline void SetSize(
			SizeTarget type,
			const vec2 newSize)
		{
			setsize(
				transform,
				{},
				type,
				newSize);
		}
		inline vec2 GetSize(SizeTarget type) 
		{ 
			return getsize(
				transform,
				type); 
		}

		//
		// Z ORDER
		//

		//Makes this widget Z order 1 unit higher than target widget
		inline void MoveAbove(OpenGL_Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| !targetWidget->isInitialized)
			{
				return;
			}

			u16 targetZOrder = targetWidget->zOrder;

			u16 newZOrder = clamp(++targetZOrder, static_cast<u16>(0), MAX_Z_ORDER);

			zOrder = newZOrder;
		}
		//Makes this widget Z order 1 unit lower than target widget
		inline void MoveBelow(OpenGL_Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| !targetWidget->isInitialized)
			{
				return;
			}

			u16 targetZOrder = targetWidget->zOrder;

			//skip if target z order already is 0
			if (targetZOrder == 0) return;

			u16 newZOrder = clamp(--targetZOrder, static_cast<u16>(0), MAX_Z_ORDER);

			zOrder = newZOrder;
		}

		inline void SetZOrder(u16 newZOrder)
		{
			u16 clamped = clamp(newZOrder, static_cast<u16>(0), MAX_Z_ORDER);

			zOrder = clamped;
		}
		inline u16 GetZOrder() const { return zOrder; }

		//
		// INTERACTION
		//

		//Skip hit testing and event polling if true
		inline void SetInteractableState(bool newValue) { isInteractable = newValue; }
		//Skip hit testing and event polling if true
		inline bool IsInteractable() const { return isInteractable; }

		//If the cursor is over this widget and this widget is not
		//covered entirely or partially by another widget then this returns true
		bool IsHovered(vec2 mousePos) const;

		//Accepts mouse buttons for pressed, released, held and dragged events.
		//Use 'SetMouseHoverEvent()' and 'SetMouseScrollEvent()' to assign those events
		inline void SetMouseEvent(
			const function<void()>& newValue,
			MouseButton mouseButton,
			ActionTarget actionTarget)
		{
			if (!newValue
				|| mouseButton == MouseButton::M_INVALID)
			{
				return;
			}

			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:
			{
				event.keyPressed = KeyboardButton::K_INVALID;
				event.mousePressed = mouseButton;

				event.function_button_pressed = newValue;
				break;
			}
			case ActionTarget::ACTION_RELEASED:
			{
				event.keyReleased = KeyboardButton::K_INVALID;
				event.mouseReleased = mouseButton;

				event.function_button_released = newValue;
				break;
			}
			case ActionTarget::ACTION_HELD:
			{
				event.keyHeld = KeyboardButton::K_INVALID;
				event.mouseHeld = mouseButton;

				event.function_button_held = newValue;
				break;
			}
			case ActionTarget::ACTION_DRAGGED:
			{
				event.mouseDragged = mouseButton;

				event.function_mouse_dragged = newValue;
				break;
			}
			}
		}
		//Assigns mouse hovered event
		inline void SetMouseHoverEvent(const function<void()>& newValue)
		{
			if (!newValue) return;
			event.function_mouse_hovered = newValue;
		}
		//Assigns mouse scrolled event
		inline void SetMouseScrollEvent(const function<void()>& newValue)
		{
			if (!newValue) return;
			event.function_mouse_scrolled = newValue;
		}
		//Returns which mouse button is attached to what event, ignores hovered and scrolled events
		inline MouseButton GetMouseEventButton(ActionTarget actionTarget) const
		{
			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:  return event.mousePressed;
			case ActionTarget::ACTION_RELEASED: return event.mouseReleased;
			case ActionTarget::ACTION_HELD:     return event.mouseHeld;
			case ActionTarget::ACTION_DRAGGED:  return event.mouseDragged;
			}

			return MouseButton::M_INVALID;
		}

		//Accepts keyboard keys for pressed, released and held events, ignores all other events
		inline void SetKeyEvent(
			const function<void()>& newValue,
			KeyboardButton key,
			ActionTarget actionTarget)
		{
			if (!newValue
				|| key == KeyboardButton::K_INVALID)
			{
				return;
			}

			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED: 	event.function_button_pressed = newValue; break;
			case ActionTarget::ACTION_RELEASED: event.function_button_released = newValue; break;
			case ActionTarget::ACTION_HELD:     event.function_button_held = newValue; break;
			}
		}
		//Returns which key is attached to what key event, ignores dragged, hovered and scrolled events
		inline KeyboardButton GetKeyEventButton(ActionTarget actionTarget) const
		{
			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:  return event.keyPressed;
			case ActionTarget::ACTION_RELEASED: return event.keyReleased;
			case ActionTarget::ACTION_HELD:     return event.keyHeld;
			}

			return KeyboardButton::K_INVALID;
		}

		//Clears target event function and its buttons
		inline void ClearEvent(ActionTarget actionTarget)
		{
			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:
			{
				event.keyPressed = KeyboardButton::K_INVALID;
				event.mousePressed = MouseButton::M_INVALID;
				event.function_button_pressed = nullptr;

				break;
			}
			case ActionTarget::ACTION_RELEASED:
			{
				event.keyReleased = KeyboardButton::K_INVALID;
				event.mouseReleased = MouseButton::M_INVALID;
				event.function_button_released = nullptr;

				break;
			}
			case ActionTarget::ACTION_HELD:
			{
				event.keyHeld = KeyboardButton::K_INVALID;
				event.mouseHeld = MouseButton::M_INVALID;
				event.function_button_held = nullptr;

				break;
			}
			case ActionTarget::ACTION_DRAGGED:
			{
				event.mouseDragged = MouseButton::M_INVALID;
				event.function_mouse_dragged = nullptr;

				break;
			}
			case ActionTarget::ACTION_HOVERED:  event.function_mouse_hovered = nullptr; break;
			case ActionTarget::ACTION_SCROLLED: event.function_mouse_scrolled = nullptr; break;
			}
		}
		//Removes all event functions and resets their attached buttons
		inline void ClearAllEvents()
		{
			event.keyPressed = KeyboardButton::K_INVALID;
			event.mousePressed = MouseButton::M_INVALID;
			event.function_button_pressed = nullptr;

			event.keyReleased = KeyboardButton::K_INVALID;
			event.mouseReleased = MouseButton::M_INVALID;
			event.function_button_released = nullptr;

			event.keyHeld = KeyboardButton::K_INVALID;
			event.mouseHeld = MouseButton::M_INVALID;
			event.function_button_held = nullptr;

			event.mouseDragged = MouseButton::M_INVALID;
			event.function_mouse_dragged = nullptr;

			event.function_mouse_hovered = nullptr;

			event.function_mouse_scrolled = nullptr;
		}

		//Poll the events that have attached functions once this frame,
		//skipped internally if 'isInteractable' is false
		void PollEvents();

		//
		// GRAPHICS
		//

		inline void SetNormalizedColor(const vec3& newValue)
		{
			f32 clampX = clamp(newValue.x, 0.0f, 1.0f);
			f32 clampY = clamp(newValue.y, 0.0f, 1.0f);
			f32 clampZ = clamp(newValue.z, 0.0f, 1.0f);

			render.color = vec3(clampX, clampY, clampZ);
		}
		inline void SetRGBColor(const vec3& newValue)
		{
			int clampX = clamp(static_cast<int>(newValue.x), 0, 255);
			int clampY = clamp(static_cast<int>(newValue.y), 0, 255);
			int clampZ = clamp(static_cast<int>(newValue.z), 0, 255);

			f32 normalizedX = static_cast<f32>(clampX) / 255;
			f32 normalizedY = static_cast<f32>(clampY) / 255;
			f32 normalizedZ = static_cast<f32>(clampZ) / 255;

			render.color = vec3(normalizedX, normalizedY, normalizedZ);
		}

		inline const vec3& GetNormalizedColor() const { return render.color; }
		inline vec3 GetRGBColor() const
		{
			int rgbX = static_cast<int>(render.color.x * 255);
			int rgbY = static_cast<int>(render.color.y * 255);
			int rgbZ = static_cast<int>(render.color.z * 255);

			return vec3(rgbX, rgbY, rgbZ);
		}

		inline void SetOpacity(f32 newValue)
		{
			f32 clamped = clamp(newValue, 0.0f, 1.0f);
			render.opacity = clamped;
		}
		inline f32 GetOpacity() const { return render.opacity; }

		inline u32 GetVAO() const { return render.VAO; }
		inline u32 GetVBO() const { return render.VBO; }
		inline u32 GetEBO() const { return render.EBO; }

		inline const OpenGL_Shader* GetShader() const { return render.shader; }

		inline void SetTexture(OpenGL_Texture* newTexture)
		{
			if (newTexture
				&& render.texture != newTexture)
			{
				render.texture = newTexture;
			}
		}
		inline void ClearTexture() { render.texture = nullptr; }
		inline const OpenGL_Texture* GetTexture() const { return render.texture; }

		//Do not destroy manually, erase from registry instead
		virtual ~OpenGL_Widget() = 0;
	protected:
		virtual void UpdateAABB(f32 viewportHeight) = 0;

		bool isInitialized{};

		string name = "NO_NAME_ADDED";

		u32 ID{};
		u32 windowID{};
		u32 glID{};

		bool isHovered{};

		HitTarget hitTarget{};

		u16 zOrder{};

		bool isInteractable = true;

		Transform2D transform{};
		OpenGL_Widget_Render render{};
		OpenGL_Widget_Event event{};

		static void CreateWidgetGeometry(
			const vector<vec2>& vertices,
			const vector<u32>& indices,
			const vector<u32>& uvs,
			u32& vaoOut,
			u32& vboOut,
			u32& eboOut);
	};
}