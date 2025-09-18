# Elypso Engine Documentation

## Overview
Elypso Engine is a lightweight, modern C++20 game engine built on top of Kalawindow.  
It provides the core systems and APIs required for game development, while staying lean, predictable, and open source.  
The engine is distributed as a **DLL with headers** for use in both games and the Elypso Editor.

---

## Responsibilities
- Provide all core systems for game development:
  - Scene management.
  - GameObject and Component system.
  - Asset loading and management.
  - Rendering and shader framework (via Kalawindow).
  - Audio playback (via Kalawindow).
  - Input handling (via Kalawindow).
- Serialize and deserialize scene data (`.escn`).
- Compile projects into standalone games through the build pipeline.

---

## Architecture
- **Engine DLL**  
  - Core runtime systems exposed via API.  
  - Shared between Elypso Editor and games.  
  - No editor UI code is included in the DLL.

- **Editor**  
  - Built on top of the Elypso Engine DLL.  
  - Provides developer-facing UI: inspector, scene hierarchy, project hierarchy, and property editors.  
  - Used for authoring and editing scenes and projects.  

- **Games**  
  - Generated from template projects.  
  - Link against the Elypso Engine DLL.  
  - Developers implement gameplay logic in C++20, extending the engine API.  

---

## Scene System
- **Scene File (`.escn`)**  
  - Stores scene metadata, GameObject hierarchy, and per-object component data.  
  - Does not embed assets; references are stored as paths.  
  - Binary format for efficiency, with optional text representation for early development.  

- **Scene Lifecycle**  
  - Scenes are created via `Scene::create()` (from file path or empty).  
  - The engine owns all scenes as `unique_ptr`.  
  - Removing a scene triggers RAII destructors, ensuring clean teardown of GameObjects and Components.  
  - Only one scene is active at a time in the engine/editor.  
  - Games may load/unload scenes as needed at runtime.  

---

## GameObject and Component Model
- **GameObject**  
  - Core container for components.  
  - Stores ID, name, and a list of components.  
  - Managed via `unique_ptr` ownership by the scene.

- **Components**  
  - Owned by their GameObject as `unique_ptr`.  
  - Encapsulate specific mechanics (transform, renderer, physics, audio, etc.).  
  - Expose APIs via getters and setters.  
  - Inheritance-based model rather than strict ECS.  

- **Ownership Model**  
  - Strict RAII with `unique_ptr` across all layers (Scene → GameObject → Component).  
  - No `shared_ptr` or `weak_ptr`.  
  - Ensures deterministic cleanup and avoids memory leaks.  

- **Enable/Disable System**  
  - GameObjects and Components can be enabled or disabled.  
  - Disabling an object unloads its assets.  
  - Hierarchical: disabling a parent disables all its children.  

---

## Asset Management
- Assets are referenced by path in `.escn`.  
- On build, Elypso Engine compiles each scene into:  
  - `.escn` (scene data).  
  - `.kdat` (assets used in that scene).  
- **`.kdat`**  
  - Custom binary archive format provided by Kaladata.  
  - Stores name, path, and file data.  
  - Deduplication supported: if an asset exists in another scene’s `.kdat`, a reference is stored instead of duplicating.  
- Runtime behavior:  
  - In the editor, all assets for the current scene are loaded up front.  
  - In games, developers control which assets are enabled/disabled (and thus loaded/unloaded).  

---

## Project Workflow
1. **Create Project** via Elypso Hub (`.eproj` generated).  
2. **Build Scenes** in Elypso Editor, saved as `.escn` + `.kdat`.  
3. **Compile Game** from template project:  
   - Links against Elypso Engine DLL.  
   - Includes gameplay code written in C++20.  
   - Produces standalone executable.  

---

## Licensing
- Elypso Engine is **FOSS** (open source).  
- Source is available on GitHub.  
- Distributed under a license that allows open source use but restricts commercial distribution of the engine itself.  
- No paywalls, subscriptions, or hidden fees.  