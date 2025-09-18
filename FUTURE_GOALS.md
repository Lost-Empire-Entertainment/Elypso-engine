# Elypso Hub Documentation

## Overview
Elypso Hub is the project and engine version manager for Elypso Engine.  
Its primary purpose is to manage projects, handle engine versions, and launch projects.  
It is **not** part of the engine or game runtime — it operates as a separate tool.

---

## Responsibilities
- Manage Elypso projects (`.eproj` files).
- Manage Elypso Engine versions:
  - Enforce project–engine version compatibility.
  - Download and install engine versions.
- Launch projects:
  - Start the Elypso Editor.
  - Start the compiled game build.
- Lock projects to supported versions to prevent corruption or undefined behavior.

---

## `.eproj` (Elypso Project File)
- Stores:
  - Metadata (project name, author, Elypso Engine version).
  - Project settings (global render/audio/input defaults, startup scene).
  - Build settings (platform, output path, build configuration).
- Does **not** embed scene files.
- Version metadata:
  - **Build type**: Debug or Release.
  - **Release state**: Pre-release (source exists but no binaries distributed) or Release.
  - **Maturity state**: Alpha, Beta, or Stable.
- Elypso Hub uses this information to enforce compatibility and warn the developer of project stability.

---

## Version Management
- `.eproj` files store the Elypso Engine version they were created with.
- Elypso Hub will:
  - Refuse to run a project with an unsupported Elypso Engine version.
  - Manage multiple versions side by side.
  - Allow developers to fetch older or newer versions as required.
- Projects **may be backward compatible**, depending on changes made between versions.
- In Alpha, breaking changes are possible and expected.

---

## Distribution
- **Source code**: Entirely open source on GitHub (hub, editor, engine).
- **Releases**:
  - Elypso Hub distributed as a prebuilt executable.
  - Elypso Editor distributed as a prebuilt executable.
  - Elypso Engine distributed as a prebuilt DLL with API headers (outside the scope of Elypso Hub).
- Developers can use prebuilt releases without building from source.
- Advanced users may fork, extend, or build their own versions directly from source.