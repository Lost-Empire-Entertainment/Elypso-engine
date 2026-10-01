# Build shaders

KalaGraphics requires built shaders since it has a fixed ecosystem of features. The shader source files are found in `files/shaders/rasterized`. To compile them from source simply run `glslc test_rs.vert -o test_rs_vert.spv`.

## Shader list

| Name | Compiled name | Type | Path |
|------|---------------|------|------|
| test_rs.vert | test_rs_vert.spv | Vertex shader for testing rasterized pipeline color | `files/shaders/rasterized` |
| test_rs.frag | test_rs_frag.spv | Fragment shader for testing rasterized pipeline color | `files/shaders/rasterized` |
