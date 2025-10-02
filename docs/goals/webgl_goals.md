# WebGL Build Workflow (Elypso + Kalawindow)

This document outlines the **core workflow** for building and distributing WebGL applications using **Elypso Engine** and **Kalawindow**.

---

## Libraries Involved

### Kalawindow
- **Description**: Low-level C++ API for windowing, input, audio, and rendering.  
- **Use Case**: Acts as the foundation layer. All rendering (OpenGL 3.3 / Vulkan 1.3 / WebGL) and system interaction goes through Kalawindow.  
- **WebGL Role**: Provides an abstraction over WebGL via Emscripten so the same C++ code runs in a browser.

### Elypso Engine
- **Description**: High-level game engine built on Kalawindow. Includes editor UI, asset pipeline, project management, and export tooling.  
- **Use Case**: For developers who want a full Unity-style workflow but in C++20.  
- **WebGL Role**: Automates the Emscripten compilation pipeline and packaging for web exports.

### Emscripten
- **Description**: LLVM-based compiler toolchain that compiles C++ into **WebAssembly (WASM)** and translates OpenGL calls to **WebGL**.  
- **Use Case**: Bridge between native C++ code and the browser environment.  
- **WebGL Role**: Produces the `.wasm`, `.js`, and `index.html` files needed to run Elypso/Kalawindow projects in the browser.

### WebView2 (Windows only)
- **Description**: Lightweight embedding of the Edge (Chromium) engine into native applications.  
- **Use Case**: Runs the same WASM/WebGL build in a standalone desktop window without launching a separate browser.  
- **WebGL Role**: Provides a desktop wrapper for WebGL builds, making them behave like standard programs.

---

## Workflow Overview

### 1. Development
- Code is written in **C++** against Kalawindow/Elypso APIs.  
- Developers never need to touch HTML, CSS, or JavaScript.  

---

### 2. Compilation
- Elypso Hub invokes **Emscripten** to compile the project.  
- Output bundle (standard WebGL format):  
  - `index.html` → minimal loader stub  
  - `game.wasm` → compiled C++ → WebAssembly  
  - `game.js` → Emscripten runtime glue  
  - `assets/` → packaged assets (e.g. .edat/.epak)  

---

### 3. Build Types
- **Standalone Desktop App (WebView2)**  
  - Wraps the WebGL build inside an `.exe`.  
  - Opens in its own window without launching the system browser.  
  - Ideal for distributing games/tools as desktop software.  

- **Web Assets**  
  - Produces the raw folder with `index.html + wasm + js + assets/`.  
  - Upload directly to itch.io or host on a website.  
  - Runs in any modern browser (Chrome, Edge, Firefox, Safari).  

---

### 4. Runtime
- **Browser (itch.io / hosted site):**  
  - Browser loads `index.html`.  
  - Emscripten initializes WASM.  
  - Kalawindow routes input/audio/rendering through WebGL.  

- **Standalone (WebView2):**  
  - User launches `.exe`.  
  - WebView2 window opens internally.  
  - Loads the same `index.html` invisibly.  
  - Runs identically to browser mode, but as a desktop app.  

---

### 5. Distribution
- **Web Assets** → zip the output folder and upload to itch.io or your own web server.  
- **Standalone App** → distribute the `.exe` directly as a desktop program.  

---
