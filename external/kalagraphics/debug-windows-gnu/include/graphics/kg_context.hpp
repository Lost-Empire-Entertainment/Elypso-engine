//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <array>
#include <functional>

#include "core_utils.hpp"
#include "math_utils.hpp"
#include "key_standards.hpp"

#include "core/kg_registry.hpp"

struct VkInstance_T;
using VkInstance = VkInstance_T*;

struct VkSurfaceKHR_T;
using VkSurfaceKHR = VkSurfaceKHR_T*;

struct VkPhysicalDevice_T;
using VkPhysicalDevice = VkPhysicalDevice_T*;

struct VkDevice_T;
using VkDevice = VkDevice_T*;

struct VkQueue_T;
using VkQueue = VkQueue_T*;

struct VkDescriptorPool_T;
using VkDescriptorPool = VkDescriptorPool_T*;

struct VmaAllocator_T;
using VmaAllocator = VmaAllocator_T*;

struct VkSwapchainKHR_T;
using VkSwapchainKHR = VkSwapchainKHR_T*;

struct VkRenderPass_T;
using VkRenderPass = VkRenderPass_T*;

struct VkImage_T;
using VkImage = VkImage_T*;

struct VmaAllocation_T;
using VmaAllocation = VmaAllocation_T*;

struct VkImageView_T;
using VkImageView = VkImageView_T*;

struct VkFramebuffer_T;
using VkFramebuffer = VkFramebuffer_T*;

struct VkSemaphore_T;
using VkSemaphore = VkSemaphore_T*;

struct VkFence_T;
using VkFence = VkFence_T*;

struct VkCommandPool_T;
using VkCommandPool = VkCommandPool_T*;

struct VkCommandBuffer_T;
using VkCommandBuffer = VkCommandBuffer_T*;

namespace KalaGraphics::Graphics
{
    using KalaHeaders::KalaMath::vec2;

    using KalaHeaders::KalaKeyStandards::KeyboardButton;
    using KalaHeaders::KalaKeyStandards::MouseButton;

    using KalaGraphics::Core::KalaGraphicsRegistry;

    using std::string;
    using std::string_view;
    using std::vector;
    using std::array;
    using std::function;
    using std::default_delete;

    static constexpr u8 MAX_FRAMES_IN_FLIGHT = 2;

    //Max total descriptor sets this pool can allocate at once,
    //shared across every Mesh, Camera and Texture descriptor set
    static constexpr u32 MAX_DESCRIPTOR_SETS = 4096;

    //Mesh matrix UBO + camera matrix UBO
    static constexpr u32 MAX_UNIFORM_BUFFER_DESCRIPTORS = 2048;
    //Texture sampler bindings
    static constexpr u32 MAX_COMBINED_IMAGE_SAMPLER_DESCRIPTORS = 2048;
    //For future compute/SSBO-based features
    static constexpr u32 MAX_STORAGE_BUFFER_DESCRIPTORS = 2048;
    //For future bindless-style setups
    static constexpr u32 MAX_SAMPLER_DESCRIPTORS = 64;

    enum class Severity : u8
    {
        SEVERITY_INFO = 0,
        SEVERITY_WARNING = 1,
        SEVERITY_FATAL = 2
    };

    enum class VSyncState : u8
	{
        //Lowest latency, no tearing (mailbox, fifo_relaxed/fifo as fallback)
        VSYNC_ON_TRIPLE_BUFFERED = 0,

        //Frames synced to display refresh rate to prevent stuttering,
        //can cause tearing (fifo_relaxed, fifo as fallback)
        VSYNC_ON_ADAPTIVE = 1,

        //Uncapped framerates, no waiting, causes tearing (immediate)
        VSYNC_OFF = 2
	};

    struct LIB_API GraphicsContextData
    {
        u32 windowID{};

#if defined(KWIN_ANY)
        uintptr_t context_window{};
#else
        uintptr_t context_display{};
        uintptr_t context_window{};
#endif

        VkSurfaceKHR context_vk_surface{};
    };

    class LIB_API GraphicsContext
    {
    friend class HitTest;
    friend class Viewport;
    friend class Shader;
    friend class Camera;
    friend class Texture;
    friend class Mesh;
    friend struct default_delete<GraphicsContext>;
    public:
        KNODISCARD
		static KalaGraphicsRegistry<GraphicsContext>& GetRegistry();

        //Close the program, this close function is useful for
        //printing the VkResult error type that occured so it can be logged
        static void ForceClose(
            string_view title,
            string_view reason,
            int result);

        KNODISCARD
		static bool IsVerboseLoggingEnabled();
        static void SetVerboseLoggingState(bool state);

        KNODISCARD
		static string GetVkResultMessage(int result);
        KNODISCARD
		static Severity GetVkResultSeverity(int result);

        static u32 GetModifierChar();
        static void SetModifierChar(u32 newValue);

        static bool GetBackspaceState();
        static void SetBackspaceState(bool newValue);

        static bool GetTabState();
        static void SetTabState(bool newValue);

        static bool GetLeftArrowState();
        static void SetLeftArrowState(bool newValue);

        static bool GetRightArrowState();
        static void SetRightArrowState(bool newValue);

        static bool GetUpArrowState();
        static void SetUpArrowState(bool newValue);

        static bool GetDownArrowState();
        static void SetDownArrowState(bool newValue);

        static const vector<KeyboardButton>& GetHeldKeys();
        static void SetHeldKeys(const vector<KeyboardButton>& newValue);

        static const vector<KeyboardButton>& GetPressedKeys();
        static void SetPressedKeys(const vector<KeyboardButton>& newValue);

        static const vector<KeyboardButton>& GetReleasedKeys();
        static void SetReleasedKeys(const vector<KeyboardButton>& newValue);

        static const vector<MouseButton>& GetHeldMouseButtons();
        static void SetHeldMouseButtons(const vector<MouseButton>& newValue);

        static const vector<MouseButton>& GetPressedMouseButtons();
        static void SetPressedMouseButtons(const vector<MouseButton>& newValue);

        static const vector<MouseButton>& GetReleasedMouseButtons();
        static void SetReleasedMouseButtons(const vector<MouseButton>& newValue);

        static const vector<MouseButton>& GetDoubleClickedMouseButtons();
        static void SetDoubleClickedMouseButtons(const vector<MouseButton>& newValue);

        static const vector<MouseButton>& GetDraggingMouseButtons();
        static void SetDraggingMouseButtons(const vector<MouseButton>& newValue);

        static f32 GetScrollWheelDelta();
        static void SetScrollWheelDelta(f32 newValue);

        //Global one-time Vulkan 1.4 device init,
        //needs to be called before per-window Vulkan init
        static void Initialize(VkInstance vkInstance);
        KNODISCARD
		static bool IsInitialized();

        //What happens BEFORE KalaGraphics hit testing and graphics context logic
        static void EarlyUpdate(const function<void()>& globalEarlyUpdate = {});

		//What happens IN BETWEEN KalaGraphics hit testing and graphics context logic
        static void Update(const function<void()>& globalUpdate = {});

        //What happens AFTER KalaGraphics hit testing and graphics context logic
        static void LateUpdate(const function<void()>& globalLateUpdate = {});

        //Initialize a per-window Vulkan context, creates the swapchain logic,
        //always internally initalizes a root viewport so each graphics context has a surface ready to be drawn onto
        KNODISCARD
		static GraphicsContext* InitializeInstance(GraphicsContextData&& context);

        KNODISCARD
		u32 GetID() const;
        KNODISCARD
        u32 GetHitTestID() const;

        KNODISCARD
        u32 GetRootViewportID() const;
        KNODISCARD
		const vector<u32>& GetExtraViewportIDs() const;

        KNODISCARD
		VSyncState GetVSyncState() const;
        void SetVSyncState(VSyncState newValue);

        //Get current Windows/X11 window true window size,
        //this is also used as VkExtent
        KNODISCARD
		vec2 GetRenderSize() const;
    
        KNODISCARD
		const GraphicsContextData& GetGraphicsContextData() const;

        //Full update call for this context, do not call manually every frame,
        //it is already called internally every frame, exposed for moments where
        //calling more than once per frame is necessary,
        //returns true if it recreated the swapchain
        KNODISCARD
        bool _UpdateInstance();

        //Do not call once per frame, only call alongside _UpdateInstance
        //if _UpdateInstance needs to be called more than once per frame 
        void _RecreateSwapchain();

        //Cheap, can be called as much as you like,
        //asks if recreateswapchain has already been called,
        //if it has then this is ignored and reset at the end of the frame
        void RequestRecreateSwapchain();

        vec2 GetMousePos(bool yReversed);
        void SetMousePos(vec2 pos);

		//What happens before per-context logic
		//but after global early update
		void SetEarlyUpdateCallback(function<void()>&& newValue);
		//What happens after this context logic
		//but before global update
		void SetUpdateCallback(function<void()>&& newValue);
		//What happens after global update but before global late update
		void SetLateUpdateCallback(function<void()>&& newValue);

        void Destroy();
    private:
        ~GraphicsContext();

        KNODISCARD
		static VkInstance GetInstance();

        KNODISCARD
		static VkPhysicalDevice GetPhysicalDevice();
        KNODISCARD
		static VkDevice GetLogicalDevice();
        KNODISCARD
		static VmaAllocator GetVmaAllocator();
        KNODISCARD
		static VkDescriptorPool GetDescriptorPool();        

        KNODISCARD
		static u32 GetDefaultColorFormat();
        KNODISCARD
		static u32 GetDefaultDepthFormat();

        void InitializeVulkanContext();

        //Create and use a single time command buffer for a small batch of operations
        KNODISCARD
		VkCommandBuffer BeginSingleTimeCommands();
        //Destroy and stop using the created command buffer
        void EndSingleTimeCommands(VkCommandBuffer vkCommandBuffer);

        void SortViewports();

        void HandleResult(
            i32 result,
            const string& reason);

        u32 ID{};
        u32 hitTestID{};
        u32 rootViewportID{};

        bool isViewportSortDirty{};

        bool requestedSwapchainRecreation{};
        bool swapchainWasRecreated{};

        //viewports that use this graphics context
        vector<u32> extraViewportIDs{};

        VSyncState vsyncState = VSyncState::VSYNC_ON_TRIPLE_BUFFERED;

        GraphicsContextData contextData{};

        size_t currentFrame{};

        vec2 oldRenderSize{};
        vec2 renderSize{};

        vec2 mousePos = -1;

		function<void()> earlyUpdateCallback{};
		function<void()> updateCallback{};
		function<void()> lateUpdateCallback{};

        VkSwapchainKHR swapchain{};
        u32 swapchainFormat{};
        vector<VkFence> swapchainImagesInFlight{};
        vector<VkImage> swapchainImages{};
        vector<VkImageView> swapchainImageViews{};
        VkImage depthImage{};
        VmaAllocation depthAllocation{};
        VkImageView depthImageView{};
        array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> availableSemaphores{};
        vector<VkSemaphore> renderFinishedSemaphores{};
        VkCommandPool commandPool{};
        array<VkFence, MAX_FRAMES_IN_FLIGHT> inFlightFences{};
        array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> commandBuffers{};
    };
}