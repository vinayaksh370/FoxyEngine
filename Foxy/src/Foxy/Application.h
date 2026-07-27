// Foxy/src/Foxy/Application.h

#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

// Define the Vulkan dynamic dispatcher - this needs to occur in exactly one cpp file in the program.
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#define GLFW_INCLUDE_NONE // Don't use OpenGL
//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <nvrhi/nvrhi.h>
#include <nvrhi/vulkan.h>
#include <nvrhi/validation.h>

#include "vk_types.h"

namespace Foxy
{
    // Application Settings - Window Specifications Defaults
    struct ApplicationSpecification
    {
        std::string Name = "Foxy App"; 
        uint32_t Width = 1600;         
        uint32_t Height = 900;         
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& specification);
        //~Application();
    public:
        void Run();

    private:
        // App Lifecycle Functions
        void initWindow(); // Create the GLFW window
        void initVulkan(); // Set up Vulkan rendering
        void mainLoop();   // The main game/draw loop
        void cleanup();    // Clean up when closing

        // Window Settings
        GLFWwindow* m_Window = nullptr; // The actual window we draw on
        bool m_FramebufferResized = false; // Set by GLFW's resize callback, checked in drawFrame()
        ApplicationSpecification m_AppSpec;

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        vk::raii::Context m_Context;                                 // Loads its own constructer
        vk::raii::Instance m_Instance = nullptr;                     // Connection to Vulkan 
        vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr; // Error catcher 

        // Vulkan setup helpers
        void createInstance();                                    // Create the Vulkan connection
        void setupDebugMessenger();                               // Set up the error catcher
        bool checkValidationLayerSupport();                       // Check if error catcher is available
        std::vector<const char*> getRequiredInstanceExtensions(); // Get needed features

        // Graphics Card Selection
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE; // The chosen graphics card
        vk::raii::PhysicalDevice m_ChosenGPU = nullptr;
        void pickPhysicalDevice();                      // Choose a graphics card
        bool isDeviceSuitable(const vk::raii::PhysicalDevice& device); // Check if card is good enough

        // The features we need from our graphics card
        const std::vector<char const*> kRequiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME // Needed to show images on screen
        };

        // --------------------------------------------
        // Logical Device and Queues
        // --------------------------------------------
        VkDevice m_Device = VK_NULL_HANDLE;       // Our "connection" to the chosen GPU
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE; // Where we submit graphics commands

        // Deliberate deviation from the tutorial: the tutorial uses a throwaway
        // local `graphicsIndex` inside createLogicalDevice() and discards it.
        // We keep it as a persistent member because NVRHI's DeviceDesc later
        // needs the actual queue family index (int), not just the VkQueue handle.
        int m_GraphicsQueueFamily = -1;

        void createLogicalDevice(); // Create the logical device + get its queue

        // --------------------------------------------
        // Window Surface
        // --------------------------------------------
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // The surface we render/present into

        void createSurface(); // Create the window surface

        // --------------------------------------------
        // Swap Chain
        // --------------------------------------------
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        std::vector<VkImage> m_SwapChainImages; // The actual images we render into
        VkSurfaceFormatKHR m_SwapChainSurfaceFormat{};
        VkExtent2D m_SwapChainExtent{};

        void createSwapChain();
        void cleanupSwapChain();  // Destroys image views + swapchain (not the surface/device)
        void recreateSwapChain(); // Waits for a valid size, then rebuilds the swapchain + image views

        // Swap chain helper functions
        static uint32_t chooseSwapMinImageCount(const VkSurfaceCapabilitiesKHR& capabilities);
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities); // not static, needs m_Window

        // --------------------------------------------
        // Image Views
        // --------------------------------------------
        std::vector<VkImageView> m_SwapChainImageViews;

        void createImageViews();

        // --------------------------------------------
        // Graphics Pipeline
        // --------------------------------------------
        void createGraphicsPipeline(); // Build the programmable shader stages for the pipeline

        VkShaderModule
        createShaderModule(const std::vector<char>& code) const; // Wrap SPIR-V bytecode in a shader module
        static std::vector<char>
        readFile(const std::string& filename); // Load a binary file (e.g. compiled shader) into memory

        VkPipelineLayout m_PipelineLayout =
            VK_NULL_HANDLE; // Describes uniform/push-constant layout for the pipeline (empty for now)

        VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE; // The actual baked pipeline object

        // --------------------------------------------
        // Command Pool / Command Buffer
        // --------------------------------------------
        static constexpr int kMaxFramesInFlight = 2;

        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_CommandBuffers; // One per frame-in-flight; freed automatically when the pool is destroyed

        void createCommandPool();
        void createCommandBuffers();
        void recordCommandBuffer(uint32_t imageIndex);

        // Transitions a swap chain image between layouts (e.g. undefined -> color attachment -> present)
        void transitionImageLayout(uint32_t imageIndex, VkImageLayout oldLayout, VkImageLayout newLayout,
                                   VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
                                   VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlags2 dstStageMask);

        // --------------------------------------------
        // Synchronization
        // --------------------------------------------
        // m_PresentCompleteSemaphores and m_InFlightFences are indexed by m_FrameIndex (one per frame-in-flight).
        // m_RenderFinishedSemaphores is indexed by imageIndex (one per swapchain image) - a frame-in-flight isn't
        // guaranteed to always land on the same swapchain image, so this needs its own per-image semaphore.
        std::vector<VkSemaphore> m_PresentCompleteSemaphores; // Signaled when a swapchain image is ready to render into
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;  // Signaled when rendering is done, safe to present
        std::vector<VkFence> m_InFlightFences; // Signaled when the GPU is done with a given frame-in-flight's work
        uint32_t m_FrameIndex = 0;             // Cycles 0..kMaxFramesInFlight-1 every drawFrame() call

        void createSyncObjects();
        void drawFrame();

        // --------------------------------------------
        // Vertex Buffer
        // --------------------------------------------
        VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;

        void createVertexBuffer();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        // --------------------------------------------
        // Debug/Validation - Like having a teacher check our work
        // --------------------------------------------
        // 
        // func catches Vulkan mistakes
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            vk::DebugUtilsMessageTypeFlagsEXT messageType,
            const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, 
            void* pUserData)
        {
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
            return vk::False; // Tell Vulkan "I saw the error, keep going"
        }

        // List of (validation layers) we want to use
        const std::vector<char const*> kValidationLayers = {"VK_LAYER_KHRONOS_validation"};

// Turn on Validation Layers only when DEBUG mode
#ifdef NDEBUG
        static constexpr bool kEnableValidationLayers = false;
#else
        static constexpr bool kEnableValidationLayers = true;
#endif














        // --------------------------------------------
        // NVRHI Experiment (isolated — creates an nvrhi::IDevice wrapper
        // around our existing raw-Vulkan device, but nothing uses it yet.
        // Not part of the render loop. See context file for the plan.)
        // --------------------------------------------
        nvrhi::DeviceHandle m_NvrhiDevice;

        void createNvrhiDeviceExperiment();
    };

} // namespace Foxy



// #pragma once
//
// #include "Layer.h"
//
// #include <string>
// #include <vector>
// #include <memory>
// #include <functional>
//
// #include "imgui.h"
// #include "vulkan/vulkan.h"
//
// void check_vk_result(VkResult err);
//
// struct GLFWwindow;
//
// namespace Walnut {
//
//	struct ApplicationSpecification
//	{
//		std::string Name = "Walnut App";
//		uint32_t Width = 1600;
//		uint32_t Height = 900;
//	};
//
//	class Application
//	{
//	public:
//		Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
//		~Application();
//
//		static Application& Get();
//
//		void Run();
//		void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }
//
//		template<typename T>
//		void PushLayer()
//		{
//			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
//			m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
//		}
//
//		void PushLayer(const std::shared_ptr<Layer>& layer) { m_LayerStack.emplace_back(layer); layer->OnAttach(); }
//
//		void Close();
//
//		float GetTime();
//		GLFWwindow* GetWindowHandle() const { return m_WindowHandle; }
//
//		static VkInstance GetInstance();
//		static VkPhysicalDevice GetPhysicalDevice();
//		static VkDevice GetDevice();
//
//		static VkCommandBuffer GetCommandBuffer(bool begin);
//		static void FlushCommandBuffer(VkCommandBuffer commandBuffer);
//
//		static void SubmitResourceFree(std::function<void()>&& func);
//	private:
//		void Init();
//		void Shutdown();
//	private:
//		ApplicationSpecification m_Specification;
//		GLFWwindow* m_WindowHandle = nullptr;
//		bool m_Running = false;
//
//		float m_TimeStep = 0.0f;
//		float m_FrameTime = 0.0f;
//		float m_LastFrameTime = 0.0f;
//
//		std::vector<std::shared_ptr<Layer>> m_LayerStack;
//		std::function<void()> m_MenubarCallback;
//	};
//
//	// Implemented by CLIENT
//	Application* CreateApplication(int argc, char** argv);
// }
