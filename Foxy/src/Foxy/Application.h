// Foxy/src/Foxy/Application.h

#pragma once

// Due to VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS being defined, eErrorOutOfDateKHR can be checked as a result
// here and does not need to be caught by an exception. // This was the solution adviced by vulkann tutorial but it does not seem to work [Known Bug]
//#define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS 1

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

//#define VK_NO_PROTOTYPES 

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>



#define GLFW_INCLUDE_NONE // Don't use OpenGL
#define GLFW_INCLUDE_VULKAN
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
        uint32_t Width   = 1600;         
        uint32_t Height  = 900;         
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
        GLFWwindow* m_Window = nullptr;         // The actual window we draw on
        bool m_FramebufferResized = false;      // Set by GLFW's resize callback, checked in drawFrame()
        ApplicationSpecification m_AppSpec;

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        // Vulkan Stuff

        vk::detail::DynamicLoader         m_dynamicLoader;
        void initDispatchLoader();

        vk::raii::Context                 m_Context;                                        // Loads its own constructer
        vk::raii::Instance                m_Instance = nullptr;               // Connection to Vulkan API
        vk::raii::DebugUtilsMessengerEXT  m_DebugMessenger = nullptr;         // Error catcher 
        vk::raii::PhysicalDevice          m_ChosenGPU = nullptr;              // Selected PhysicalDevice or GPU
        vk::raii::SurfaceKHR              m_Surface = nullptr;                // The surface we render/present into
        vk::raii::Device                  m_Device = nullptr;                 // LogicalDevice :: Our "connection" to the chosen GPU
        vk::raii::Queue                   m_GraphicsQueue = nullptr;          // Where we submit graphics commands
        int                               m_GraphicsQueueFamily = -1;         // unchanged - plain int, never was a Vulkan 

        // Nvrhi Stuff
        nvrhi::DeviceHandle               m_NvrhiDevice;
        std::vector<nvrhi::TextureHandle> m_NvrhiSwapChainImages;

        vk::raii::SwapchainKHR            m_SwapChain = nullptr;
        std::vector<vk::Image>            m_SwapChainImages;
        vk::SurfaceFormatKHR              m_SwapChainSurfaceFormat;
        vk::Extent2D                      m_SwapChainExtent;
        std::vector<vk::raii::ImageView>  m_SwapChainImageViews;


        vk::raii::PipelineLayout          m_PipelineLayout = nullptr;         // Describes uniform/push-constant layout (empty for now)
        vk::raii::Pipeline                m_GraphicsPipeline = nullptr;       // The actual baked pipeline object

        static constexpr int kMaxFramesInFlight = 2;

        // Command Pool/Buffers
        vk::raii::CommandPool                m_CommandPool = nullptr;
        std::vector<vk::raii::CommandBuffer> m_CommandBuffers; // One per frame-in-flight; freed automatically when the pool is destroyed

        // Synchronization
        std::vector<vk::raii::Semaphore> m_PresentCompleteSemaphores; // Signaled when a swapchain image is ready to render into
        std::vector<vk::raii::Semaphore> m_RenderFinishedSemaphores;  // Signaled when rendering is done, safe to present
        std::vector<vk::raii::Fence>     m_InFlightFences;            // Signaled when the GPU is done with a given frame-in-flight's work
        uint32_t                         m_FrameIndex = 0;            // Cycles 0..kMaxFramesInFlight-1 every drawFrame() call

        // The features we need from our graphics card
        const std::vector<char const*> kRequiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME                          // Needed to show images on screen
        };

        // Vulkan setup helpers
        void createInstance();                                         // Create the Vulkan connection
        void setupDebugMessenger();                                    // Set up the error catcher
        bool checkValidationLayerSupport();                            // Check if error catcher is available

        void pickPhysicalDevice();                                     // Prefer Dedicated ; FallBack Integrated
        bool isDeviceSuitable(const vk::raii::PhysicalDevice& device); // Check if card is good enough

        void createLogicalDevice();                                    // Create the logical device + get its queue
        void createSurface();                                          // Create the window surface

        // SwapChain
        void createSwapChain();
        void cleanupSwapChain();
        void recreateSwapChain();

        void createImageViews();                                       // Image Views

        // Graphics Pipeline
        void createGraphicsPipeline();                                                    // Build the programmable shader stages for the pipeline

        vk::raii::ShaderModule   createShaderModule(const std::vector<char>& code) const; // Wrap SPIR-V bytecode in a shader module
        static std::vector<char> readFile(const std::string& filename);                   // Load a binary file (e.g. compiled shader) into memory

        // CommandPools & CommandBuffers
        void createCommandPool();
        void createCommandBuffers();
        void recordCommandBuffer(uint32_t imageIndex);

        // Synchronization

        void createSyncObjects();
        void drawFrame();

        // NVRHI STUFF
        void createNvrhiDevice();
        nvrhi::TextureHandle wrapSwapChainImageForNvrhi(vk::Image image);

        /*==========================================*/
        // Swapchain Helper Func & Other Helper Func
        /*==========================================*/

        static uint32_t             chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities);
        static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
        static vk::PresentModeKHR   chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
        vk::Extent2D                chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

        std::vector<const char*> getRequiredInstanceExtensions(); // Get needed features

        // Transitions a swap chain image between layouts (e.g. undefined -> color attachment -> present)
        void transitionImageLayout(uint32_t imageIndex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                   vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
                                   vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask);


        

        // --------------------------------------------
        // Vertex Buffer
        // --------------------------------------------
        /*VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;

        void createVertexBuffer();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);*/

        // Debug/Validation - Like having a teacher check our work
        
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
        static constexpr bool kEnableNvrhiValidationLayers = false;
#else
        static constexpr bool kEnableValidationLayers = true;
        static constexpr bool kEnableNvrhiValidationLayers = true;
#endif

    };
} // namespace Foxy


// #if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
// #include <vulkan/vulkan_raii.hpp>
// #else
// import vulkan_hpp;
// #endif
// 
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
