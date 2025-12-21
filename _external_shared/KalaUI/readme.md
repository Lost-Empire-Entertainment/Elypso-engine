# KalaUI

**KalaUI** is a retained mode production ready UI library for OpenGL 3.3 and it provides a fully self made UI framework, with a widget system where Image and Text are the root widgets that does not own or create a window or the GL context.

It is recommended to use [KalaWindow](https://github.com/kalakit/kalawindow) to get the GL context and the GL functions but any other window context initializer and GL library like GLFW should work as well.

---

## Supported platforms

| Platform  | Status         | Notes                                                 |
|-----------|----------------|-------------------------------------------------------|
| Windows   | Supported      | Recommended Windows 10 (build 1809 / 17763) or higher |
| Linux     | Planned        | Support coming in a future version                    |
| WebGL     | Planned        | Support coming in a future version                    |
| Mac       | Not Supported  | No plans for macOS compatibility                      |
| ARM       | Not Supported  | ARM architecture will not be targeted                 |
| Console   | Not Supported  | No plans for console platforms                        |
| Mobile    | Not Supported  | No plans for mobile platforms                         |

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

[Lost Empire Entertainment and KalaKit ecosystem](docs/ecosystem.md)