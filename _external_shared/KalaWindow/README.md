# KalaWindow

**KalaWindow** is a C++20 window library for Windows and Linux, used for making software and applications.

**KalaWindow** creates and owns the OpenGL 3.3 context and Vulkan 1.3 context, manages the message loop, handles crashes safely through the crash handler and handles all inputs through the input framework. Glad or any other opengl function loaders are not required because **KalaWindow** already provides most common ones for what you would need for OpenGL or Vulkan.

---

## Supported platforms

| Platform  | Status         | Notes                                                |
|-----------|----------------|------------------------------------------------------|
| Windows   | Supported      | Requires Windows 10 (build 1809 / 17763) or higher   |
| Linux     | Supported      | Should work on any Linux distro released in the last ~10 years ago |
| WebGL     | Planned        | Support coming in a future version                   |
| Mac       | Not Supported  | No plans for macOS compatibility                     |
| ARM       | Not Supported  | ARM architecture will not be targeted                |
| Console   | Not Supported  | No plans for console platforms                       |
| Mobile    | Not Supported  | No plans for mobile platforms                        |

---

## Supported render pipelines

| Render Pipeline | Status         | Notes                                             |
|-----------------|----------------|---------------------------------------------------|
| OpenGL          | Supported      | Internally locked to OpenGL 3.3                   |
| Vulkan          | Planned        | Support for Vulkan 1.3 coming in a future version |
| DirectX         | Not Supported  | No plans for DirectX compatibility                |
| MoltenVK        | Not Supported  | No plans for MoltenVK compatibility               |

---

## Links

[Donate on PayPal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Official Discord server](https://discord.gg/jkvasmTND5)

[Official Youtube channel](https://youtube.com/greenlaser)

---

## Docs

[How to build from source](docs/build_from_source.md)

[External libraries](docs/external_libraries.md)

[Minimum requirements](docs/minimum_requirements.md)

[Lost Empire Entertainment and KalaKit ecosystem](docs/ecosystem.md)