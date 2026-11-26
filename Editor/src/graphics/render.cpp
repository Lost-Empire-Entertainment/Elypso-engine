//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"
#include "math_utils.hpp"
#include "key_standards.hpp"
#include "export_glb.hpp"

#include "core/ee_core.hpp"
#include "import/ee_import_data.hpp"
#include "export/ee_export_data.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kg_context.hpp"
#include "graphics/kg_hit_test.hpp"
#include "graphics/kg_camera.hpp"
#include "graphics/kg_material.hpp"
#include "graphics/kg_texture.hpp"
#include "graphics/kg_mesh.hpp"
#include "widgets/kg_text.hpp"
#include "import/kg_import_font.hpp"
#include "import/kg_import_mesh.hpp"

#include "core/core.hpp"
#include "graphics/render.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::epsilon;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::vec4;
using KalaHeaders::KalaMath::quat;
using KalaHeaders::KalaMath::Transform2D;
using KalaHeaders::KalaMath::Transform3D;
using KalaHeaders::KalaMath::PosTarget;
using KalaHeaders::KalaMath::RotTarget;
using KalaHeaders::KalaMath::SizeTarget;

using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::MouseButton;

using KalaHeaders::KalaExportGLB::ExportMeshData;
using KalaHeaders::KalaExportGLB::ExportGLBTextureData;
using KalaHeaders::KalaExportGLB::ExportMaterialData;
using KalaHeaders::KalaExportGLB::ExportNodeData;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Import::AssetData;
using ElypsoEngine::Import::ImportData;
using ElypsoEngine::Export::ExportData;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::FileType;
using KalaWindow::Graphics::Window_Global;
using KalaGraphics::Graphics::VSyncState;
using KalaGraphics::Graphics::GraphicsContext;
using KalaGraphics::Graphics::ViewportType;
using KalaGraphics::Graphics::RootShaderTarget;
using KalaGraphics::Graphics::Camera;
using KalaGraphics::Graphics::AlphaMode;
using KalaGraphics::Graphics::Material;
using KalaGraphics::Graphics::TextureFilterMode;
using KalaGraphics::Graphics::TextureWrapMode;
using KalaGraphics::Graphics::TextureData;
using KalaGraphics::Graphics::Texture;
using KalaGraphics::Graphics::Vertex;
using KalaGraphics::Graphics::AnchorPosition;
using KalaGraphics::Graphics::Mesh_Cube;
using KalaGraphics::Graphics::MeshData;
using KalaGraphics::Graphics::Mesh;
using KalaGraphics::Widgets::Text;
using KalaGraphics::Import::ImportFont;
using KalaGraphics::Import::ImportNodeData;
using KalaGraphics::Import::ImportPrimitiveData;
using KalaGraphics::Import::ImportMaterialData;
using KalaGraphics::Import::ImportMesh;

using ElypsoEditor::Core::ElypsoEditorCore;
using ElypsoEditor::Graphics::TextData;

using std::string;
using std::to_string;
using std::vector;
using std::filesystem::path;

static bool isInitialized{};

static bool isCameraMovable{};

static ImportFont* font{};

static Texture* vp1_Tex_root{};
static Texture* vp1_Tex_fallback{};
static Texture* vp1_Tex_fallback_groundTest{};

static vector<Mesh*> vp1_Mesh3D_importedMeshes{};
static Material* vp1_Material_groundText{};
static Mesh* vp1_Mesh3D_groundTest{};

static vector<TextData> vp1_Text_all{};

static path lastImportedMeshPath{};
static vector<Mesh*> lastImportedMeshPtrs{};

namespace ElypsoEditor::Graphics
{
    bool Render::IsInitialized() { return isInitialized; }
    void Render::Initialize()
    {
        if (!ElypsoEditorCore::IsInitialized())
        {
            Log::Print(
                "Failed to initialize renderer because core has not yet been initialized!",
                "EE_RENDER",
                LogType::LOG_WARNING);

            return;
        }
        if (isInitialized)
        {
            Log::Print(
                "Failed to initialize renderer because it has already been initialized!",
                "EE_RENDER",
                LogType::LOG_WARNING);

            return;
        }

        //sync before creating kg objects
        EngineCore::SyncID();

        const path& exePath = KalaWindowCore::GetExePath();

        Viewport* viewport = ElypsoEditorCore::GetViewport();
        Shader* primary3DShader = ElypsoEditorCore::Get3DShader();
        Input* input = ElypsoEditorCore::GetInput();

        viewport->SetType(ViewportType::FILL);

        //
        // CREATE TEXTURES
        //

        Texture* fallbackTex{};
        string err = Texture::GetRegistry().GetContent(primary3DShader->GetFallbackTextureID(), fallbackTex);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to get fallback texture from shader '" + to_string(primary3DShader->GetID()) + "'! Reason: " + err);
        }

        vp1_Tex_fallback_groundTest = CreateTexture(
            primary3DShader,
            {
                .pixelData = vector<u8>(fallbackTex->GetPixelData()),
                .filterMode = TextureFilterMode::NEAREST,
                .wrapMode = TextureWrapMode::REPEAT,
                .size = 16
            });

        //
        // CREATE GROUND
        //

        vp1_Material_groundText = Material::Initialize(primary3DShader->GetID());
        vp1_Mesh3D_groundTest = CreateMesh(
            vp1_Material_groundText,
            Mesh::GenerateMeshData(Mesh_Cube{.edgeCount = 4}));

        vp1_Material_groundText->SetBaseColorTextureID(vp1_Tex_fallback_groundTest->GetID());

        vector<Vertex> vertices = vp1_Mesh3D_groundTest->GetVertices();
        vector<u32> indices = vp1_Mesh3D_groundTest->GetIndices();

        vec3 newSize = { 10.0f, epsilon, 10.0f }; 

        for (Vertex& v : vertices)
        {
            v.uv.x *= newSize.x;
            v.uv.y *= newSize.z;
        }

        vp1_Mesh3D_groundTest->SetMeshData(
        {
            .vertices = std::move(vertices),
            .indices = std::move(indices)
        });
        Transform3D& groundTransform = scast<Transform3D&>(vp1_Mesh3D_groundTest->GetTransform());
        groundTransform.setsize(newSize);
        groundTransform.update();

        //
        // SELECT AND INITIALIZE FONT
        //

        //initialize from selected font path

        /*
        vector<path> files = Window_Global::GetFiles(
            FileType::FILE_CUSTOM,
            {
                ".ttf",
                ".otf"
                });

        if (files.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to import font because no font was selected!");
        }

        string fontName = files.front().stem().string();
        font = ImportFont::Initialize(
            path(files.front()),
            64);

        if (!font)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create font!");
        }
        */

        //initialize from known font path

        path fontName = path("LeagueGothic") / "LeagueGothic-Regular.otf";
        path fontPath = exePath.parent_path() / "files" / "fonts" / fontName;

        font = ImportFont::Initialize(
            path(fontPath),
            32);

        if (!font)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create font!");
        }

        if (!font)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to import font '" + fontName.string() + "'!");
        }

        /*
        Test_Print_Glyph_Atlas_To_Texture(
            font,
            vp1_Tex_font,
            vp1_Mesh2D_font,
            vp1_Mesh2D_fontBackground);
        */

        //
        // CREATE TEXT WIDGETS
        //

        Text* fpsText = Render::CreateText(
            font,
            viewport);

        Mesh* fpsTextMesh{};
        err = Mesh::GetRegistry().GetContent(fpsText->GetMeshID(), fpsTextMesh);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create fps text because fps text mesh was invalid!");
        }

        Text* frameLengthText = Render::CreateText(
            font,
            viewport,
            fpsTextMesh);

        Mesh* frameLengthTextMesh{};
        err = Mesh::GetRegistry().GetContent(frameLengthText->GetMeshID(), frameLengthTextMesh);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create frame length text because fps text mesh was invalid!");
        }

        Text* pointOneText = Render::CreateText(
            font,
            viewport,
            frameLengthTextMesh);

        Mesh* pointOneTextMesh{};
        err = Mesh::GetRegistry().GetContent(pointOneText->GetMeshID(), pointOneTextMesh);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create point one text because frame length text mesh was invalid!");
        }

        Text* pointZeroOneText = Render::CreateText(
            font,
            viewport,
            pointOneTextMesh);

        Mesh* pointZeroOneTextMesh{};
        err = Mesh::GetRegistry().GetContent(pointZeroOneText->GetMeshID(), pointZeroOneTextMesh);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create point zero one text because point one text mesh was invalid!");
        }

        Text* windowResText = Render::CreateText(
            font,
            viewport,
            pointZeroOneTextMesh);

        Mesh* windowResTextMesh{};
        err = Mesh::GetRegistry().GetContent(windowResText->GetMeshID(), windowResTextMesh);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create window resolution text because point zero one text mesh was invalid!");
        }

        //
        // CREATE BUTTONS
        //

        Text* importModelsButton = Render::CreateText(
            font,
            viewport);
        importModelsButton->SetText("Import glb model", true);

        TextData importModelsTextData = Render::GetTextData(importModelsButton);
        if (!importModelsTextData.text)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to get import models button text data because import models text was invalid!");
        }

        importModelsTextData.textMesh->SetViewportAnchorPosition(AnchorPosition::TOP_CENTER);
        importModelsTextData.textMesh->SetSelfAnchorPosition(AnchorPosition::TOP_CENTER);

        importModelsTextData.textBackgroundMesh->SetSelfAnchorPosition(AnchorPosition::CENTER);
        importModelsTextData.textBackgroundMesh->SetTargetAnchorPosition(
            AnchorPosition::CENTER,
            importModelsTextData.textMesh->GetID());

        importModelsTextData.textBackgroundMesh->SetIgnoreHoverState(false);
        importModelsTextData.textBackgroundMesh->SetMouseButtonPressedCallback(
            MouseButton::M_LEFT,
            [primary3DShader]() 
                { 
                    string err = Render::ImportMeshes(primary3DShader);
                    if (!err.empty())
                    {
                        Log::Print(
                            "Failed to import meshes! Reason: " + err,
                            "EE_RENDER",
                            LogType::LOG_WARNING);
                    }
                });

        //
        // FINISH
        //

        //sync after kg objects are done with initialization
        EngineCore::SyncID();

        isInitialized = true;
    }

    void Render::Update()
    {
        if (!isInitialized) return;

        static ProcessWindow* pw = ElypsoEditorCore::GetProcessWindow();
        static Input* input = ElypsoEditorCore::GetInput();
        static GraphicsContext* gctx = ElypsoEditorCore::GetGraphicsContext();
        static Viewport* viewport = ElypsoEditorCore::GetViewport();
        static Camera* camera = ElypsoEditorCore::GetCamera();

        ToggleCamera(input);

        MoveCamera(
            input,
            camera,
            EngineCore::GetDeltaTime());

        ToggleVSync(gctx, input);

        //first is always fps text
        Text* fpsText = vp1_Text_all[0].text;
        if (!fpsText)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor runtime error",
                "Failed to update FPS text because its widget was invalid!");
        }
        //second is always frame length text
        Text* frameLengthText = vp1_Text_all[1].text;
        if (!frameLengthText)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor runtime error",
                "Failed to update frame length text because its widget was invalid!");
        }
        //third is always point one text
        Text* pointOneText = vp1_Text_all[2].text;
        if (!pointOneText)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor runtime error",
                "Failed to update point one text because its widget was invalid!");
        }
        //fourth is always point zero one text
        Text* pointZeroOneText = vp1_Text_all[3].text;
        if (!pointZeroOneText)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor runtime error",
                "Failed to update point zero one text because its widget was invalid!");
        }

        static string lastFPS{};
        string fps = ElypsoEditorCore::GetFPS()[0];
        string frameLength = ElypsoEditorCore::GetFPS()[1];
        string pointOne = ElypsoEditorCore::GetFPS()[2];
        string pointZeroOne = ElypsoEditorCore::GetFPS()[3];

        if (lastFPS != ""
            && fps == "")
        {
            lastFPS = fps;

            fpsText->SetText("");
            frameLengthText->SetText("");
            pointOneText->SetText("");
            pointZeroOneText->SetText("");
        }
        if (lastFPS != fps)
        {
            lastFPS = fps;

            fpsText->SetText(fps, true);
            if (frameLengthText->GetText(true) != frameLength)
            {
                frameLengthText->SetText(frameLength, true);
            }
            if (pointOneText->GetText(true) != pointOne)
            {
                pointOneText->SetText(pointOne, true);
            }
            if (pointZeroOneText->GetText(true) != pointZeroOne)
            {
                pointZeroOneText->SetText(pointZeroOne, true);
            }
        }

        //fifth is always window resolution
        Text* winResText = vp1_Text_all[4].text;
        if (!winResText)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor runtime error",
                "Failed to update window resolution text because its widget was invalid!");
        }
        
        static string lastWinRes{};
        vec2 winResOriginal = pw->GetSize();
        string winRes = 
            "resolution: " +
            to_string(scast<u32>(winResOriginal.x)) + "x" 
            + to_string(scast<u32>(winResOriginal.y));

        if (lastWinRes != winRes)
        {
            lastWinRes = winRes;
            winResText->SetText(lastWinRes, true);
        }

        //third is always viewport resolution
        Text* vpResText = vp1_Text_all[2].text;
        if (!vpResText)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor runtime error",
                "Failed to update viewport resolution text because its widget was invalid!");
        }
        
        static string lastVPRes{};
        vec2 vpResOriginal = viewport->GetScissorSize();
        string vpRes = 
            to_string(scast<u32>(vpResOriginal.x)) + "x" 
            + to_string(scast<u32>(vpResOriginal.y));

        if (lastVPRes != vpRes)
        {
            lastVPRes = vpRes;
            vpResText->SetText(lastVPRes, true);
        }

        for (TextData& td : vp1_Text_all)
        {
            if (!td.text) continue;

            Transform2D& tr = scast<Transform2D&>(td.textMesh->GetTransform());
            Transform2D& bgtr = scast<Transform2D&>(td.textBackgroundMesh->GetTransform());

            const vec2 textSize = tr.getsize(SizeTarget::LOCAL);
            const vec2 textBackgroundSize = bgtr.getsize(SizeTarget::LOCAL);

            if (textBackgroundSize != textSize)
            {
                bgtr.setsize(textSize);
                bgtr.update();
            }
        }

        /*
        Test_Print_Glyph_To_Texture(
            input,
            font,
            vp1_Tex_font,
            vp1_Mesh2D_font,
            vp1_Mesh2D_fontBackground);
        */
    }

    Texture* Render::GetRootTexture() { return vp1_Tex_root; }
    void Render::SetRootTexture(Texture* newValue) { vp1_Tex_root = newValue; }

    Texture* Render::GetFallbackTexture() { return vp1_Tex_fallback; }
    void Render::SetFallbackTexture(Texture* newValue) { vp1_Tex_fallback = newValue; }

    Texture* Render::GetGroundTexture() { return vp1_Tex_fallback_groundTest; }
    void Render::SetGroundTexture(Texture* newValue) { vp1_Tex_fallback_groundTest = newValue; }

    void Render::ToggleVSync(
        GraphicsContext* gctx,
        Input* input)
    {
        //if (input->IsComboPressed(combo_vsync_disable))
        if (input->IsKeyPressed(KeyboardButton::K_1))
        {
            Log::Print("@@@@@ set vsync state to off");
            gctx->SetVSyncState(VSyncState::OFF);
        }
        //else if (input->IsComboPressed(combo_vsync_adaptive))
        else if (input->IsKeyPressed(KeyboardButton::K_2))
        {
            Log::Print("@@@@@ set vsync state to adaptive");
            gctx->SetVSyncState(VSyncState::ADAPTIVE);
        }
        //else if (input->IsComboPressed(combo_vsync_triple_buffered))
        if (input->IsKeyPressed(KeyboardButton::K_3))
        {
            Log::Print("@@@@@ set vsync state to triple buffered");
            gctx->SetVSyncState(VSyncState::TRIPLE_BUFFERED);
        }
    }

    void Render::ToggleCamera(Input* input)
    {
        //if (input->IsComboPressed(combo_toggle_cam_move))
        if (input->IsKeyPressed(KeyboardButton::K_ESC))
        {
            isCameraMovable = !isCameraMovable;

            if (!isCameraMovable)
            {
                input->SetMouseLockState(false);
                input->SetMouseVisibility(true);
            }
            else
            {
                input->SetMouseLockState(true);
                input->SetMouseVisibility(false);
            }

            Log::Print("@@@@@ set cam move state to: " 
                + string(isCameraMovable ? "on" : "off"));
        }
    }

    void Render::MoveCamera(
        Input* input,
        Camera* cam,
        f32 deltaTime)
    {
        if (!input)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to move camera because input was invalid!");
        }
        if (!cam)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to move camera because camera was invalid!");
        }

        static f32 sprintMultiplier = 2.0f;

        if (!isCameraMovable) return;

        vec2 mouse = input->GetRawMouseDelta();
        vec2 kb{};
        f32 vertical{};

        //forwards
        if (input->IsKeyHeld(KeyboardButton::K_W)) kb.y += 1.0f;
        //backwards
        if (input->IsKeyHeld(KeyboardButton::K_S)) kb.y -= 1.0f;
        //left
        if (input->IsKeyHeld(KeyboardButton::K_A)) kb.x -= 1.0f;
        //right
        if (input->IsKeyHeld(KeyboardButton::K_D)) kb.x += 1.0f;

        //up
        if (input->IsKeyHeld(KeyboardButton::K_E)) vertical += 1.0f;
        //down
        if (input->IsKeyHeld(KeyboardButton::K_Q)) vertical -= 1.0f;

        //sprint
        if (input->IsKeyHeld(KeyboardButton::K_LEFT_SHIFT)) deltaTime *= sprintMultiplier;

        /*
        string moveStr = "\nmouse - x: " 
            + to_string(mouse.x) + ", y: "
            + to_string(mouse.y) + "\nkb - x: "
            + to_string(kb.x) + ", y: "
            + to_string(kb.y);

        Transform3D& ct = cam->GetTransform();
        vec3 euler = toeuler3(ct.rot_world);

        string transformStr = "\npos - x: "
            + to_string(ct.pos_world.x) + ", y: "
            + to_string(ct.pos_world.y) + ", z: "
            + to_string(ct.pos_world.z) + ", \nrot - x: "
            + to_string(euler.x) + ", y: "
            + to_string(euler.y) + ", z: "
            + to_string(euler.z) + ", \nsize - x: "
            + to_string(ct.size_world.x) + ", y: "
            + to_string(ct.size_world.y) + ", z: "
            + to_string(ct.size_world.z);

        Log::Print(
            moveStr + transformStr,
            "EE_RENDER",
            LogType::LOG_VERBOSE);
        */

        cam->Move(
            mouse,
            kb,
            vertical,
            deltaTime);
    }

    path Render::GetLastImportedMeshPath() { return lastImportedMeshPath; }
    vector<Mesh*> Render::GetLastImportedMeshPointers() { return lastImportedMeshPtrs; }

    string Render::ImportMeshes(Shader* shader)
    {
        if (!shader) return "Shader was invalid!";

        vector<path> files = Window_Global::GetFiles(
            FileType::CUSTOM,
            {
                ".glb",
                ".gltf",
                ".edat"
                });

        if (files.empty()) return "No assets were selected!";

        ImportMesh* importMesh{};

        if (files.front().extension() == ".edat")
        {
            string err = ImportData::ImportAssetData(path(files.front()));
            if (!err.empty()) return err;

            vector<AssetData> assetData = ImportData::GetAssetData();
            if (assetData.empty()) return "GetAssetData returned empty vector!";

            vector<AssetData> streamedAssets{};
            err = ImportData::StreamAssets(
                { assetData.front().assetName },
                streamedAssets);

            if (!err.empty()) return err;

            importMesh = ImportMesh::Initialize(std::move(streamedAssets.front().assetData));
        }
        else importMesh = ImportMesh::Initialize(path(files.front()));

        if (!importMesh) return "ImportMesh couldn't be initialized!";

        vector<u32> meshIDs{};

        if (!lastImportedMeshPtrs.empty())
        {
            for (Mesh* m : lastImportedMeshPtrs) m->Destroy();
            lastImportedMeshPtrs.clear();

            //make sure latest model has latest content
            ImportData::ClearImportedData();
        }

        for (const ImportNodeData& nodeData : importMesh->GetMeshData())
        {
            for (const ImportPrimitiveData& primitiveData : nodeData.primitiveData)
            {
                Material* mat = Material::Initialize(shader->GetID());
                if (!mat)
                {
                    KalaWindowCore::ForceClose(
                        "Elypso Editor render error",
                        "Failed to create mesh material!");
                }

                Mesh* primitive = Mesh::Initialize(mat->GetID());

                if (!primitive)
                {
                    KalaWindowCore::ForceClose(
                        "Elypso Editor render error",
                        "Failed to import model '" + files.front().string() 
                        + "' because one of its primitives failed to initialize!");
                }

                primitive->SetMeshData(
                {
                    .vertices = vector<Vertex>(primitiveData.meshData.vertices),
                    .indices = vector<u32>(primitiveData.meshData.indices)
                });

                Transform3D& mt = primitive->GetTransform();
                mt.setpos(nodeData.transform.getpos(PosTarget::LOCAL));
                mt.setrotquat(nodeData.transform.getrotquat(RotTarget::LOCAL));
                mt.setsize(nodeData.transform.getsize(SizeTarget::LOCAL));
                mt.update();

                primitive->FlipFaceDirection();

                const ImportMaterialData& matData = primitiveData.matData;

                mat->SetBaseColor(vec4(matData.baseColor));

                if (matData.alphaMode != AlphaMode::TYPE_OPAQUE)
                {
                    mat->SetAlphaMode(matData.alphaMode);

                    if (matData.alphaMode == AlphaMode::TYPE_MASK)
                    {
                        mat->SetAlphaCutoff(matData.alphaCutoff);
                    }
                }

                if (!matData.textureData.pixelData.empty())
                {
                    Texture* meshTex = Texture::Initialize(shader->GetID());
                    if (!meshTex)
                    {
                        KalaWindowCore::ForceClose(
                            "Elypso Editor render error",
                            "Failed to create mesh texture!");
                    }

                    meshTex->SetPixelData(vector<u8>(matData.textureData.pixelData));
                    meshTex->SetSize(matData.textureData.size);
                    meshTex->SetPixelFormat(matData.textureData.pixelFormat);

                    mat->SetBaseColorTextureID(meshTex->GetID());
                }

                lastImportedMeshPtrs.push_back(primitive);

                meshIDs.push_back(primitive->GetID());
            }
        }

        if (files.front().extension() != ".edat")
        {
            lastImportedMeshPath = files.front();
        }

        static int exportIndex{};

        //temporarily exporting meshes right after import succeeds
        string err = ExportMeshes(
            lastImportedMeshPath.parent_path() 
            / string(lastImportedMeshPath.stem().string() + "_" + to_string(++exportIndex) + ".edat"),
            lastImportedMeshPtrs);

        if (!err.empty())
        {
            Log::Print(
                "Failed to export meshes! Reason: " + err,
                "EE_RENDER",
                LogType::LOG_WARNING);

            return {};
        }

        return {};
    }

    string Render::ExportMeshes(
        const path& targetPath,
        const vector<Mesh*>& meshes)
    {
        if (exists(targetPath))
        {
            return "Failed to export meshes because target path already exists!";
        }
        if (!targetPath.has_extension()
            || (targetPath.extension() != ".glb"
            && targetPath.extension() != ".edat"))
        {
            return "Failed to export meshes because target path had invalid or no extension!";
        }
        if (meshes.empty())
        {
            return "Failed to export meshes because no meshes were selected!";
        }

        for (const Mesh* m : meshes)
        {
            if (!m) return "Failed to export meshes because one or more of the meshes were invalid!";
        }

        if (targetPath.extension() == ".glb")
        {
            vector<ExportNodeData> nodeData{};

            u32 index{};
            for (Mesh* m : meshes)
            {
                ExportNodeData node{};
                node.nodeName = "model_" + to_string(index++);

                const Transform3D& t = scast<Transform3D&>(m->GetTransform());
                const vec3 pos = t.getpos(PosTarget::LOCAL);
                const quat rot = t.getrotquat(RotTarget::LOCAL);
                const vec3 size = t.getsize(SizeTarget::LOCAL);
                node.transform = 
                    {
                        .position = { pos.x, pos.y, pos.z },
                        .rotation = { rot.x, rot.y, rot.z, rot.w },
                        .size = { size.x, size.y, size.z }
                    };

                const vector<Vertex>& vertices = m->GetVertices();
                vector<u32> indices = m->GetIndices();

                vector<KalaHeaders::KalaExportGLB::Vertex> exportVertices(vertices.size());
                for (size_t i = 0; i < vertices.size(); i++)
                {
                    const Vertex& v = vertices[i];
                    KalaHeaders::KalaExportGLB::Vertex& ev = exportVertices[i];

                    ev.pos[0] = v.pos.x;
                    ev.pos[1] = v.pos.y;
                    ev.pos[2] = v.pos.z;

                    ev.normal[0] = v.normal.x;
                    ev.normal[1] = v.normal.y;
                    ev.normal[2] = v.normal.z;

                    ev.uv[0] = v.uv.x;
                    ev.uv[1] = v.uv.y;

                    ev.color[0] = v.color.x;
                    ev.color[1] = v.color.y;
                    ev.color[2] = v.color.z;
                    ev.color[3] = v.color.w;
                }

                node.meshData.vertices = std::move(exportVertices);
                node.meshData.indices = std::move(indices);

                Material* mat{};
                string err = Material::GetRegistry().GetContent(m->GetMaterialID(), mat);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Elypso Editor render error",
                        "Failed to get mesh '" + to_string(m->GetID()) 
                        + "' material because it was invalid! Reason: " + err);
                }

                Texture* tex{};
                err = Texture::GetRegistry().GetContent(mat->GetBaseColorTextureID(), tex);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Elypso Editor render error",
                        "Failed to get mesh '" + to_string(m->GetID()) 
                        + "' base color texture because it was invalid! Reason: " + err);
                }

                const vec4& baseColor = mat->GetBaseColor();
                node.matData.baseColor[0] = baseColor.x;
                node.matData.baseColor[1] = baseColor.y;
                node.matData.baseColor[2] = baseColor.z;
                node.matData.baseColor[3] = baseColor.w;

                node.matData.alphaMode = scast<KalaHeaders::KalaExportGLB::AlphaMode>(scast<u8>(mat->GetAlphaMode()));
                node.matData.alphaCutoff = mat->GetAlphaCutoff();

                node.matData.textureData.pngImageData = tex->GetPixelData();

                nodeData.push_back(std::move(node));
            }

            string err = KalaHeaders::KalaExportGLB::ExportMeshes(
                std::move(nodeData),
                targetPath);
            if (!err.empty()) return err;
        }
        else
        {
            path outPath{};
            size_t relevantMeshCount{};

            for (const Mesh* m : meshes)
            {
                for (const Mesh* lastMesh : lastImportedMeshPtrs)
                {
                    if (m->GetID() == lastMesh->GetID())
                    {
                        outPath = lastImportedMeshPath;
                        relevantMeshCount++;
                        break;
                    }
                }
            }

            if (outPath.empty())
            {
                return 
                    "Failed to export meshes because none of the given "
                    "meshes matched the last imported meshes "
                    "so their original mesh path could not be assigned!";
            }
            if (relevantMeshCount != meshes.size())
            {
                return 
                    "Failed to export meshes because one of the meshes "
                    "did not match any of the last imported meshes "
                    "so their original mesh path could not be assigned!";
            }

            string err = ExportData::ExportAssetData(
                targetPath,
                {{ 
                    .blockName = "mesh_" + lastImportedMeshPath.stem().string(),
                    .blockPath = lastImportedMeshPath
                }});
            if (!err.empty()) return err;
        }

        return "";
    }

    TextData Render::GetTextData(Text* text)
    {
        if (!text)
        {
            Log::Print(
                "Failed to get text data because text was invalid!",
                "EE_RENDER",
                LogType::LOG_WARNING);

            return {};
        }

        for (TextData td : vp1_Text_all)
        {
            if (td.text->GetID() == text->GetID()) return td;
        }

        Log::Print(
            "Failed to get text data because text was not used in editor!",
            "EE_RENDER",
            LogType::LOG_WARNING);

        return {};
    }
    Text* Render::CreateText(
        ImportFont* font,
        Viewport* viewport,
        Mesh* targetMeshAnchor)
    {
        if (!font)
        {
            Log::Print(
                "Failed to create text because font was invalid!",
                "EE_RENDER",
                LogType::LOG_WARNING);

            return {};
        }
        if (!viewport)
        {
            Log::Print(
                "Failed to create text because viewport was invalid!",
                "EE_RENDER",
                LogType::LOG_WARNING);

            return {};
        }

        //sync ids before generating text
        EngineCore::SyncID();

        TextData textData{};

        textData.text = Text::Initialize(
            font->GetID(),
            viewport->GetID());

        //textData.text->SetEditState(true);
        textData.text->SetMaxCharacters(50);

        string err = Mesh::GetRegistry().GetContent(textData.text->GetMeshID(), textData.textMesh);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create text because text mesh was invalid!");
        }

        //textData.textMesh->SetIgnoreHoverState(false);

        if (!targetMeshAnchor)
        {
            textData.textMesh->SetViewportAnchorPosition(AnchorPosition::TOP_LEFT);
            textData.textMesh->SetSelfAnchorPosition(AnchorPosition::TOP_LEFT);
        }
        else
        {
            textData.textMesh->SetSelfAnchorPosition(AnchorPosition::TOP_LEFT);
            textData.textMesh->SetTargetAnchorPosition(
                AnchorPosition::BOTTOM_LEFT,
                targetMeshAnchor->GetID());
        }

        Material* textMeshMat{};
        err = Material::GetRegistry().GetContent(textData.textMesh->GetMaterialID(), textMeshMat);
        if (!err.empty())
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create text because text material was invalid!");
        }
        textMeshMat->SetBaseColor( vec4{0.0f, 0.0f, 0.0f, 1.0f} );
        textMeshMat->SetAlphaMode(AlphaMode::TYPE_BLEND);

        Material* textBackgroundMeshMat = Material::Initialize(viewport->GetRootShaderID(RootShaderTarget::RECT));
        if (!textBackgroundMeshMat)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create text because text background material failed to be initialized!");
        }

        textData.textBackgroundMesh = Mesh::Initialize(textBackgroundMeshMat->GetID());
        if (!textData.textBackgroundMesh)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create text because text background mesh failed to be initialized!");
        }

        textData.textBackgroundMesh->SetSelfAnchorPosition(
            AnchorPosition::TOP_LEFT);
        textData.textBackgroundMesh->SetTargetAnchorPosition(
            AnchorPosition::TOP_LEFT,
            textData.textMesh->GetID());

        textBackgroundMeshMat->SetBaseColor( vec4{0.5f, 0.5f, 0.8f, 0.3f} );
        textBackgroundMeshMat->SetAlphaMode(AlphaMode::TYPE_BLEND);

        vp1_Text_all.push_back(textData);
        return vp1_Text_all.back().text;
    }

    Texture* Render::CreateTexture(
        Shader* shader,
        TextureData&& textureData)
    {
        if (!shader)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create texture because shader was invalid!");
        }

        //sync ids before generating texture
        EngineCore::SyncID();

        Texture* texture = Texture::Initialize(
            shader->GetID(),
            std::move(textureData));

        if (!texture)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create texture!");
        }

        return texture;
    }

    Mesh* Render::CreateMesh(
        Material* mat,
        MeshData&& meshData)
    {
        if (!mat)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create mesh because material was invalid!");
        }

        //sync ids before generating mesh
        EngineCore::SyncID();

        Mesh* mesh = Mesh::Initialize(mat->GetID());

        if (!mesh)
        {
            KalaWindowCore::ForceClose(
                "Elypso Editor render error",
                "Failed to create mesh!");
        }

        if (!mesh->Is2D()) mesh->SetMeshData(std::move(meshData));

        return mesh;
    }
}