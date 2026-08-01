//#pragma once
//
// //#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
//#include <vulkan/vulkan_raii.hpp>
//#else
//import vulkan_hpp;
//#endif
//#include <vulkan/vulkan.h>
//#include "Foxy/Application.h"
//
//namespace
//{
//    // NVRHI wants a message callback object to report its own internal
//    // errors/warnings through, separate from the Vulkan validation layer.
//    class NvrhiMessageCallback : public nvrhi::IMessageCallback
//    {
//    public:
//        void message(nvrhi::MessageSeverity severity, const char* messageText) override
//        {
//            std::cerr << "NVRHI: " << messageText << std::endl;
//        }
//    };
//
//    NvrhiMessageCallback s_NvrhiMessageCallback;
//} // namespace
//
//namespace Foxy
//{
//    // Create an nvrhi::IDevice wrapper around our existing raw Vulkan device,
//    // then optionally wrap that in NVRHI's own validation layer (Debug only,
//    // mirroring kEnableValidationLayers). Purely experimental at this stage —
//    // nothing in Foxy uses m_NvrhiDevice for rendering yet.
//    void Application::createNvrhiDeviceExperiment()
//    {
//        nvrhi::vulkan::DeviceDesc deviceDesc{};
//        deviceDesc.errorCB = &s_NvrhiMessageCallback;
//        deviceDesc.physicalDevice = m_PhysicalDevice;
//        deviceDesc.device = m_Device;
//        deviceDesc.graphicsQueue = m_GraphicsQueue;
//        deviceDesc.graphicsQueueIndex = m_GraphicsQueueFamily;
//        //deviceDesc.deviceExtensions = const_cast<char**>(kRequiredDeviceExtensions.data());
//        deviceDesc.deviceExtensions = (kRequiredDeviceExtensions.data());
//        deviceDesc.numDeviceExtensions = kRequiredDeviceExtensions.size();
//
//        m_NvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);
//
//        if (!m_NvrhiDevice)
//        {
//            std::cout << "[NVRHI experiment] Device wrapper creation FAILED." << std::endl;
//            return;
//        }
//
//        std::cout << "[NVRHI experiment] Device wrapper created successfully." << std::endl;
//
//        if (kEnableValidationLayers)
//        {
//            nvrhi::DeviceHandle validationLayer = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
//            m_NvrhiDevice = validationLayer; // route everything through the validation layer from here on
//            std::cout << "[NVRHI experiment] Validation layer active." << std::endl;
//        }
//    }
//} // namespace Foxy
//

// Foxy/src/Foxy/Application.h
//#pragma once
//
//#define GLFW_INCLUDE_NONE
//#define GLFW_INCLUDE_VULKAN
//
//#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.h>
//
//#include <nvrhi/nvrhi.h>
//#include <nvrhi/vulkan.h>
//#include <nvrhi/validation.h>
//
//#include <algorithm>
//#include <cstdint>
//#include <fstream>
//#include <iostream>
//#include <string>
//#include <vector>
//
//namespace Foxy
//{
//    struct ApplicationSpecification
//    {
//        static inline std::string Name = "Foxy Engine";
//        static constexpr uint32_t Width = 1600;
//        static constexpr uint32_t Height = 900;
//    };
//
//    class Application
//    {
//    public:
//        void Run();
//
//    private:
//        void initWindow();
//        void initVulkan();
//        void mainLoop();
//        void cleanup();
//
//        GLFWwindow* m_Window = nullptr;
//        bool m_FramebufferResized = false;
//
//        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
//
//        VkInstance m_Instance = VK_NULL_HANDLE;
//        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
//
//        void createInstance();
//        void setupDebugMessenger();
//        bool checkValidationLayerSupport();
//        std::vector<const char*> getRequiredInstanceExtensions();
//
//        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
//
//        void pickPhysicalDevice();
//        bool isDeviceSuitable(VkPhysicalDevice device);
//
//        const std::vector<char const*> kRequiredDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
//
//        VkDevice m_Device = VK_NULL_HANDLE;
//        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
//        int m_GraphicsQueueFamily = -1;
//
//        void createLogicalDevice();
//
//        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
//
//        void createSurface();
//
//        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
//        std::vector<VkImage> m_SwapChainImages;
//        VkSurfaceFormatKHR m_SwapChainSurfaceFormat{};
//        VkExtent2D m_SwapChainExtent{};
//
//        void createSwapChain();
//        void cleanupSwapChain();
//        void recreateSwapChain();
//
//        static uint32_t chooseSwapMinImageCount(const VkSurfaceCapabilitiesKHR& capabilities);
//        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
//        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
//        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
//
//        std::vector<VkImageView> m_SwapChainImageViews;
//
//        void createImageViews();
//
//        void createGraphicsPipeline();
//
//        VkShaderModule createShaderModule(const std::vector<char>& code) const;
//        static std::vector<char> readFile(const std::string& filename);
//
//        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
//        VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
//
//        static constexpr int kMaxFramesInFlight = 2;
//
//        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
//        std::vector<VkCommandBuffer> m_CommandBuffers;
//
//        void createCommandPool();
//        void createCommandBuffers();
//        void recordCommandBuffer(uint32_t imageIndex);
//
//        void transitionImageLayout(uint32_t imageIndex, VkImageLayout oldLayout, VkImageLayout newLayout,
//                                   VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
//                                   VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlags2 dstStageMask);
//
//        std::vector<VkSemaphore> m_PresentCompleteSemaphores;
//        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
//        std::vector<VkFence> m_InFlightFences;
//
//        uint32_t m_FrameIndex = 0;
//
//        void createSyncObjects();
//        void drawFrame();
//
//        VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
//        VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;
//
//        void createVertexBuffer();
//        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
//
//        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
//                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//                                                            void* pUserData)
//        {
//            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
//            return VK_FALSE;
//        }
//
//        const std::vector<char const*> kValidationLayers = {"VK_LAYER_KHRONOS_validation"};
//
//#ifdef NDEBUG
//        static constexpr bool kEnableValidationLayers = false;
//#else
//        static constexpr bool kEnableValidationLayers = true;
//#endif
//    };
//} // namespace Foxy