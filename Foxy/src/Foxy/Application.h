// Foxy/src/Foxy/Application.h

#pragma once

#define GLFW_INCLUDE_NONE // Don't use OpenGL
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <cstdint>   // For exact number types
#include <string>    // For text strings
#include <vector>    // For lists/arrays
#include <iostream>  // For printing messages
#include <algorithm> // For std::clamp
#include <fstream>   // For r/w files

namespace Foxy
{
    // Application Settings - Window Specifications
    struct ApplicationSpecification
    {
        static inline std::string Name = "Foxy Engine"; // What to call our app
        static constexpr uint32_t Width = 1600;         // Window width in pixels
        static constexpr uint32_t Height = 900;         // Window height in pixels
    };

    class Application
    {
    public:
        void Run();

    private:
        // App Lifecycle Functions
        void initWindow(); // Create the GLFW window
        void initVulkan(); // Set up Vulkan rendering
        void mainLoop();   // The main game/draw loop
        void cleanup();    // Clean up when closing

        // --------------------------------------------
        // Window Settings
        // --------------------------------------------
        GLFWwindow* m_Window = nullptr; // The actual window we draw on

        // --------------------------------------------
        // Vulkan Connection
        // --------------------------------------------
        VkInstance m_Instance = VK_NULL_HANDLE;                     // Connection to Vulkan
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE; // Error catcher

        // Vulkan setup helpers
        void createInstance();                                    // Create the Vulkan connection
        void setupDebugMessenger();                               // Set up the error catcher
        bool checkValidationLayerSupport();                       // Check if error catcher is available
        std::vector<const char*> getRequiredInstanceExtensions(); // Get needed features

        // --------------------------------------------
        // Graphics Card Selection
        // --------------------------------------------
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE; // The chosen graphics card

        // Helper functions for finding a graphics card
        void pickPhysicalDevice();                      // Choose a graphics card
        bool isDeviceSuitable(VkPhysicalDevice device); // Check if card is good enough

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
        // Debug/Validation - Like having a teacher check our work
        // --------------------------------------------

        // func catches Vulkan mistakes
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,    // How serious is the error?
            VkDebugUtilsMessageTypeFlagsEXT messageType,               // What type of error?
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, // The error details
            void* pUserData                                            // Extra data (not used)
        )
        {
            // Print the error message
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
            return VK_FALSE; // Tell Vulkan "I saw the error, keep going"
        }

        // List of (validation layers) we want to use
        const std::vector<char const*> kValidationLayers = {"VK_LAYER_KHRONOS_validation"};

// Turn on Validation Layers only when DEBUG mode
#ifdef NDEBUG
        static constexpr bool kEnableValidationLayers = false;
#else
        static constexpr bool kEnableValidationLayers = true;
#endif
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
