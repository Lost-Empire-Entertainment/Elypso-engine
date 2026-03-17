# KalaGraphics

**KalaGraphics** is a C++20 window library for Windows and Linux, built for software and games with a user-friendly GL and VK agnostic API. 

**KalaGraphics** accepts a GL 1.3 or VK 3.3 context and takes over after that, all further actions are in a single user-friendly API and the backend is determined whether you choose to have VK-specifc features or not, otherwise **KalaGraphics** always defaults to OpenGL for compatibility.

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