// Foxy/src/Foxy/Application.cpp
#include "Foxy/Application.h"
#include "vk_types.h"

// Hardcoded triangle vertex data, uploaded once to m_VertexBuffer in createVertexBuffer().
// Layout must match Vertex::GetAttributeDescriptions() and basic_triangle.slang's VertexInput.
struct Vertex
{
    float position[2];
    float color[3];
};

// static const std::vector<Vertex> kVertices = {
//     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

const std::vector<Vertex> kVertices = {
    {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}}, {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

namespace
{
    // Nvrhi Validation Layer -> display error msg
    class NvrhiMessageCallback : public nvrhi::IMessageCallback
    {
    public:
        void message(nvrhi::MessageSeverity severity, const char* messageText) override
        {
            std::cerr << "NVRHI: " << messageText << std::endl;
        }
    };
    NvrhiMessageCallback s_NvrhiMessageCallback;

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func =  (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }
} // anonymous namespace

namespace Foxy
{
    Application::Application(const ApplicationSpecification& specification) : m_AppSpec(specification)
    {
        // initWindow();
    }

    void Application::Run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    void Application::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);    // Simple Resizable or not
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // Simple Resizable or not

        m_Window = glfwCreateWindow(m_AppSpec.Width, m_AppSpec.Height, m_AppSpec.Name.c_str(), nullptr, nullptr);

        glfwSetWindowUserPointer(m_Window, this);
        glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
    }

    // Called from GLFW whenever the framebuffer size changes (e.g. window resize)
    void Application::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
        app->m_FramebufferResized = true;
    }

    void Application::initVulkan()
    {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        //createNvrhiDevice(); // isolated experiment, see Section 5 of context file
        createSwapChain();
        createImageViews();
        createGraphicsPipeline();
        createCommandPool();
        //createVertexBuffer();
        createCommandBuffers();
        createSyncObjects();
    }

    void Application::mainLoop()
    {
        while (!glfwWindowShouldClose(m_Window))
        {
            glfwPollEvents();
            drawFrame();
        }
        vkDeviceWaitIdle(*m_Device); // wait for the GPU to finish before we start destroying resources
    }

    void Application::cleanup()
    {
        // m_NvrhiDevice = nullptr; // release the NVRHI device wrapper (and validation layer, if active) before tearing
        //  down the raw VkDevice it wraps

        /*for (auto semaphore : m_RenderFinishedSemaphores)
        {
            vkDestroySemaphore(m_Device, semaphore, nullptr);
        }
        for (auto semaphore : m_PresentCompleteSemaphores)
        {
            vkDestroySemaphore(m_Device, semaphore, nullptr);
        }
        for (auto fence : m_InFlightFences)
        {
            vkDestroyFence(m_Device, fence, nullptr);
        }

        vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
        vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);

        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
        vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

        cleanupSwapChain();*/

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    // CREATE INSTANCE:
    void Application::createInstance()
    {
        if (kEnableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        constexpr vk::ApplicationInfo appInfo{.pApplicationName = "IDK",
                                              .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                              .pEngineName = "IDK Engine",
                                              .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                              .apiVersion = vk::ApiVersion14};

        // Get the required layers
        std::vector<const char*> requiredLayers;
        if (kEnableValidationLayers)
        {
            requiredLayers.assign(kValidationLayers.begin(), kValidationLayers.end());
        }

        // Get the required extensions.
        auto requiredExtensions = getRequiredInstanceExtensions();

        // Check if the required extensions are supported by the Vulkan implementation.
        std::vector<vk::ExtensionProperties> extensionProperties = m_Context.enumerateInstanceExtensionProperties();
        for (const char* requiredExtension : requiredExtensions)
        {
            bool found = false;
            for (const auto& extensionProperty : extensionProperties)
            {
                if (strcmp(extensionProperty.extensionName, requiredExtension) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                throw std::runtime_error(std::string("Required extension not supported: ") + requiredExtension);
        }

        vk::InstanceCreateInfo createInfo{.pApplicationInfo = &appInfo,
                                          .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
                                          .ppEnabledLayerNames = requiredLayers.data(),
                                          .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
                                          .ppEnabledExtensionNames = requiredExtensions.data()};

        // Throws vk::SystemError on failure - no manual VK_SUCCESS check needed.
        // m_Context is what actually bootstraps m_Instance here - this is the one
        // place the Context we talked about earlier gets used directly.
        m_Instance = vk::raii::Instance(m_Context, createInfo);
    }

    bool Application::checkValidationLayerSupport()
    {
        std::vector<vk::LayerProperties> layerProperties = m_Context.enumerateInstanceLayerProperties();

        for (const char* layerName : kValidationLayers)
        {
            bool layerFound = false;
            for (const auto& layerProperty : layerProperties)
            {
                if (strcmp(layerName, layerProperty.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound)
            {
                return false;
            }
        }
        return true;
    }

    std::vector<const char*> Application::getRequiredInstanceExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (kEnableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void Application::setupDebugMessenger()
    {
        if (!kEnableValidationLayers)
        {
            return;
        }

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                                           vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

        vk::DebugUtilsMessengerCreateInfoEXT createInfo{
            .messageSeverity = severityFlags, .messageType = messageTypeFlags, .pfnUserCallback = &debugCallback};

        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(createInfo);

        std::cout << "[Vulkan] Validation layer active." << std::endl;
    }

    // PHYSICAL DEVICE -> prefer DedicatedGPU || Fallback IntegratedGPU
    void Application::pickPhysicalDevice()
    {
        std::vector<vk::raii::PhysicalDevice> physicalDevices = m_Instance.enumeratePhysicalDevices();

        if (physicalDevices.empty())
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        // Collect every suitable device instead of stopping at the first one, so we
        // can prefer a discrete GPU over an integrated one when both are present.
        std::vector<vk::raii::PhysicalDevice> suitableDevices;
        for (const auto& device : physicalDevices)
        {
            if (isDeviceSuitable(device))
            {
                suitableDevices.push_back(device);
            }
        }

        if (suitableDevices.empty())
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        // Prefer a discrete GPU (dedicated graphics card) over an integrated one.
        // Falls back to whichever suitable device came first if no discrete GPU exists.
        m_ChosenGPU = suitableDevices[0];
        for (const auto& device : suitableDevices)
        {
            if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                m_ChosenGPU = device;
                break;
            }
        }

        std::cout << "Selected GPU: " << m_ChosenGPU.getProperties().deviceName << std::endl;
    }

    // Finds if device is suitable
    bool Application::isDeviceSuitable(const vk::raii::PhysicalDevice& device)
    {
        vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
        bool supportsVulkan1_3 = deviceProperties.apiVersion >= vk::ApiVersion13;

        std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
        bool supportsGraphics = false;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                supportsGraphics = true;
                break;
            }
        }

        std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();
        bool supportsAllRequiredExtensions = true;
        for (const char* requiredExtension : kRequiredDeviceExtensions)
        {
            bool found = false;
            for (const auto& availableExtension : availableExtensions)
            {
                if (strcmp(availableExtension.extensionName, requiredExtension) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                supportsAllRequiredExtensions = false;
                break;
            }
        }

        auto features = device
                .getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                              vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

        bool supportsRequiredFeatures =
            features.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
            features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
            features.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
            features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

        return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
    }

    /* Window Surface */ 
    void Application::createSurface()
    {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(*m_Instance, m_Window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
        m_Surface = vk::raii::SurfaceKHR(m_Instance, surface);
    }

    /* Create Logical Device */
    void Application::createLogicalDevice()
    {
        std::vector<vk::QueueFamilyProperties> queueFamilies = m_ChosenGPU.getQueueFamilyProperties();

        for (uint32_t i = 0; i < queueFamilies.size(); i++)  // Go through All queuefamily || Pick first that supports both graphics and present
        {
            if ((queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                m_ChosenGPU.getSurfaceSupportKHR(i, m_Surface))
            {
                m_GraphicsQueueFamily = static_cast<int>(i);
                break;
            }
        }

        if (m_GraphicsQueueFamily == -1)
        {
            throw std::runtime_error("No queue family found that supports both graphics and present!");
        }

        // Re-request the same Vulkan 1.1 / 1.3 / extended-dynamic-state feature chain
        // used in isDeviceSuitable() - but this time SET the fields to true to actually
        // turn the features on, instead of just reading them to check support.
        vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                           vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
            featureChain = {
                {},                                                   
                {.shaderDrawParameters = true},                       
                {.synchronization2 = true, .dynamicRendering = true}, 
                {.extendedDynamicState = true}
            };

        float queuePriority = 0.5f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo{.queueFamilyIndex = static_cast<uint32_t>(m_GraphicsQueueFamily),
                                                        .queueCount = 1,
                                                        .pQueuePriorities = &queuePriority};

        vk::DeviceCreateInfo deviceCreateInfo{.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
                                              .queueCreateInfoCount = 1,
                                              .pQueueCreateInfos = &deviceQueueCreateInfo,
                                              .enabledExtensionCount = static_cast<uint32_t>(kRequiredDeviceExtensions.size()),
                                              .ppEnabledExtensionNames = kRequiredDeviceExtensions.data()};

        m_Device = vk::raii::Device(m_ChosenGPU, deviceCreateInfo);
        m_GraphicsQueue = vk::raii::Queue(m_Device, static_cast<uint32_t>(m_GraphicsQueueFamily), 0);
    }

    // NVRHI DEVICE //
    //void Application::createNvrhiDevice()
    //{
    //    std::vector<const char*> requiredLayers;
    //    if (kEnableValidationLayers)
    //    {
    //        requiredLayers.assign(kValidationLayers.begin(), kValidationLayers.end());
    //    }
    //    nvrhi::vulkan::DeviceDesc deviceDesc
    //    {
    //        .errorCB             = &s_NvrhiMessageCallback,
    //        .physicalDevice      = *m_ChosenGPU,
    //        .device              = *m_Device,
    //        .graphicsQueue       = *m_GraphicsQueue,
    //        .graphicsQueueIndex  = m_GraphicsQueueFamily,
    //        //.deviceExtensions    = const_cast<char**>(kRequiredDeviceExtensions.data()), 
    //        .deviceExtensions =   requiredLayers.data(), 
    //        .numDeviceExtensions = kRequiredDeviceExtensions.size()
    //    };
    //    m_NvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);
    //
    //    if (kEnableNvrhiValidationLayers)
    //    {
    //        nvrhi::DeviceHandle validationLayer = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
    //        m_NvrhiDevice = validationLayer; // route everything through the validation layer from here on
    //        std::cout << "[NVRHI] Validation layer active." << std::endl;
    //    }
    //}

    // SwapChain Setup //
    void Application::createSwapChain()
    {
        vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_ChosenGPU.getSurfaceCapabilitiesKHR(*m_Surface);

        m_SwapChainExtent      = chooseSwapExtent(surfaceCapabilities);
        uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

        std::vector<vk::SurfaceFormatKHR> availableFormats = m_ChosenGPU.getSurfaceFormatsKHR(*m_Surface);
        m_SwapChainSurfaceFormat                           = chooseSwapSurfaceFormat(availableFormats);

        std::vector<vk::PresentModeKHR> availablePresentModes = m_ChosenGPU.getSurfacePresentModesKHR(*m_Surface);
        vk::PresentModeKHR presentMode                        = chooseSwapPresentMode(availablePresentModes);

        vk::SwapchainCreateInfoKHR swapChainCreateInfo{
            .surface          = *m_Surface,
            .minImageCount    = minImageCount,
            .imageFormat      = m_SwapChainSurfaceFormat.format,
            .imageColorSpace  = m_SwapChainSurfaceFormat.colorSpace,
            .imageExtent      = m_SwapChainExtent,
            .imageArrayLayers = 1,
            .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode = vk::SharingMode::eExclusive, // Foxy always has one combined graphics+present  queue family (enforced in isDeviceSuitable) —// no second queue to share images with.
            .preTransform     = surfaceCapabilities.currentTransform,
            .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode      = presentMode,
            .clipped          = true};

        m_SwapChain = vk::raii::SwapchainKHR(m_Device, swapChainCreateInfo);
        m_SwapChainImages = m_SwapChain.getImages();

    }

    // Destroy everything tied to the current swap chain, without touching the surface/device.
    void Application::cleanupSwapChain()
    {
        m_SwapChainImageViews.clear(); // raii - destructors run here, no manual vkDestroyImageView needed
        m_SwapChain = nullptr;         // raii - destructor runs here, no manual vkDestroySwapchainKHR needed
    }


    // Rebuild the swap chain + image views (e.g. after a resize or VK_ERROR_OUT_OF_DATE_KHR)
    void Application::recreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);
        while (width == 0 || height == 0) // window is minimized - wait until it isn't
        {
            glfwGetFramebufferSize(m_Window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(*m_Device); // don't touch resources the GPU might still be using

        cleanupSwapChain();
        createSwapChain();
        createImageViews();
    }

    // Nvrhi Wrapper for SwapChainImages
    nvrhi::TextureHandle Application::wrapSwapChainImageForNvrhi(vk::Image image)
    {
        nvrhi::TextureDesc textureDesc;
        textureDesc.width = m_SwapChainExtent.width;
        textureDesc.height = m_SwapChainExtent.height;
        textureDesc.format =
            nvrhi::Format::BGRA8_UNORM; // hardcoded mapping - would need a real vk::Format -> nvrhi::Format table
        textureDesc.isRenderTarget = true;
        textureDesc.initialState = nvrhi::ResourceStates::Present;
        textureDesc.keepInitialState = true;
        textureDesc.debugName = "SwapChainImage";

        return m_NvrhiDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image,
                                                           nvrhi::Object(static_cast<VkImage>(image)), textureDesc);
    }

    // Create SwapChain Images Views //
    void Application::createImageViews()
    {
        assert(m_SwapChainImageViews.empty());

        vk::ImageViewCreateInfo imageViewCreateInfo{.viewType = vk::ImageViewType::e2D,
                                                    .format = m_SwapChainSurfaceFormat.format,
                                                    .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};

        m_SwapChainImageViews.reserve(m_SwapChainImages.size());
        for (const auto& image : m_SwapChainImages)
        {
            imageViewCreateInfo.image = image;
            m_SwapChainImageViews.emplace_back(m_Device, imageViewCreateInfo);
        }
    }

    

    void Application::createGraphicsPipeline()
    {
        // LOAD SHADER //
        vk::raii::ShaderModule shaderModule = createShaderModule(readFile("shaders/basic_triangle.spv"));

        vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
            .stage  = vk::ShaderStageFlagBits::eVertex, 
            .module = shaderModule, 
            .pName  = "vertMain"};
        vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
            .stage  = vk::ShaderStageFlagBits::eFragment, 
            .module = shaderModule, 
            .pName  = "fragMain"};

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        // VERTEX INFO
         vk::PipelineVertexInputStateCreateInfo   vertexInputInfo;
         vk::PipelineInputAssemblyStateCreateInfo inputAssembly{.topology = vk::PrimitiveTopology::eTriangleList};
         vk::PipelineViewportStateCreateInfo      viewportState{.viewportCount = 1, .scissorCount = 1};

        // NOTE: still an empty vertex input state - vertex buffer wiring is deliberately
        // deferred for now (per Vinu's instruction), so this doesn't describe the Vertex
        // struct's binding/attribute layout yet. Revisit alongside the vertex buffer work.

        // RASTERIZER -> Converts Triangles to Fragments //
        vk::PipelineRasterizationStateCreateInfo rasterizer{.depthClampEnable        = vk::False,
                                                            .rasterizerDiscardEnable = vk::False,
                                                            .polygonMode             = vk::PolygonMode::eFill,
                                                            .cullMode                = vk::CullModeFlagBits::eBack,
                                                            .frontFace               = vk::FrontFace::eClockwise,
                                                            .depthBiasEnable         = vk::False,
                                                            .lineWidth               = 1.0f};

        // MULTISAMPLING -> Disabled //
        vk::PipelineMultisampleStateCreateInfo multisampling{.rasterizationSamples = vk::SampleCountFlagBits::e1,
                                                             .sampleShadingEnable  = vk::False};
        // COLOR BLENDING -> Disabled //
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            .blendEnable = vk::False,
            .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};

        vk::PipelineColorBlendStateCreateInfo colorBlending{.logicOpEnable   = vk::False,
                                                            .logicOp         = vk::LogicOp::eCopy,
                                                            .attachmentCount = 1,
                                                            .pAttachments    = &colorBlendAttachment};

        // DYNAMIC STATE //

        std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

        vk::PipelineDynamicStateCreateInfo dynamicState{.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), 
                                                        .pDynamicStates    = dynamicStates.data()};

        // Gpraphics Pipeline Setup

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{.setLayoutCount = 0, .pushConstantRangeCount = 0};
        m_PipelineLayout = vk::raii::PipelineLayout(m_Device, pipelineLayoutInfo);

        vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
            {.stageCount          = 2,
             .pStages             = shaderStages,
             .pVertexInputState   = &vertexInputInfo,
             .pInputAssemblyState = &inputAssembly,
             .pViewportState      = &viewportState,
             .pRasterizationState = &rasterizer,
             .pMultisampleState   = &multisampling,
             .pColorBlendState    = &colorBlending,
             .pDynamicState       = &dynamicState,
             .layout              = m_PipelineLayout,
             .renderPass          = nullptr},
            {.colorAttachmentCount = 1, .pColorAttachmentFormats = &m_SwapChainSurfaceFormat.format}};

        m_GraphicsPipeline = vk::raii::Pipeline(m_Device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());

        // KNOWN DEBT (unchanged): "shaders/basic_triangle.spv" is a hardcoded, app-specific
        // asset path living inside the Core/engine layer (Foxy::Application). This is exactly
        // the problem the upcoming Layer system is meant to solve.
    }

    // Shader Module - Wraps SpirV byteCode in ShaderModule
    vk::raii::ShaderModule Application::createShaderModule(const std::vector<char>& code) const
    {
        vk::ShaderModuleCreateInfo createInfo{.codeSize = code.size(),
                                              .pCode = reinterpret_cast<const uint32_t*>(code.data())};
        return vk::raii::ShaderModule(m_Device, createInfo);
    }

    // Reads a binary file as it is -> reads everything into std::vector<char>
    std::vector<char> Application::readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file: " + filename);
        }

        std::vector<char> buffer(static_cast<size_t>(file.tellg()));
        file.seekg(0);
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        file.close();

        return buffer;
    }

    // CommandPool Setup //
    void Application::createCommandPool()
    {
        vk::CommandPoolCreateInfo poolInfo{.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                           .queueFamilyIndex = static_cast<uint32_t>(m_GraphicsQueueFamily)};

        m_CommandPool = vk::raii::CommandPool(m_Device, poolInfo);
    }

    // Create Command Buffers //
    void Application::createCommandBuffers()
    {
        m_CommandBuffers.clear();
        vk::CommandBufferAllocateInfo allocInfo{.commandPool = m_CommandPool,
                                                .level = vk::CommandBufferLevel::ePrimary,
                                                .commandBufferCount = static_cast<uint32_t>(kMaxFramesInFlight)};
        m_CommandBuffers = vk::raii::CommandBuffers(m_Device, allocInfo);
    }

    // Record Command Buffer
    // Record all drawing commands for one swap chain image //
    void Application::recordCommandBuffer(uint32_t imageIndex)
    {
        auto& commandBuffer = m_CommandBuffers[m_FrameIndex];

        // Begin Buffer
        commandBuffer.begin({});

        // Transition swap chain image: undefined -> color attachment optimal
        transitionImageLayout(imageIndex, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, {},
                              vk::AccessFlagBits2::eColorAttachmentWrite,
                              vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                              vk::PipelineStageFlagBits2::eColorAttachmentOutput);

        vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
        vk::RenderingAttachmentInfo attachmentInfo{.imageView = m_SwapChainImageViews[imageIndex],
                                                   .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
                                                   .loadOp = vk::AttachmentLoadOp::eClear,
                                                   .storeOp = vk::AttachmentStoreOp::eStore,
                                                   .clearValue = clearColor};

        vk::RenderingInfo renderingInfo{.renderArea = {.offset = {0, 0}, .extent = m_SwapChainExtent},
                                        .layerCount = 1,
                                        .colorAttachmentCount = 1,
                                        .pColorAttachments = &attachmentInfo};

        commandBuffer.beginRendering(renderingInfo);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_GraphicsPipeline);
        commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_SwapChainExtent.width),
                                                  static_cast<float>(m_SwapChainExtent.height), 0.0f, 1.0f));
        commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), m_SwapChainExtent));

        // Vertex buffer setup untouched (still raw VkBuffer) - vk::Buffer implicitly converts from
        // VkBuffer since it's a handle type, so this still compiles as-is against the raii command buffer.
        //commandBuffer.bindVertexBuffers(0, {m_VertexBuffer}, {0});

        commandBuffer.draw(static_cast<uint32_t>(kVertices.size()), 1, 0, 0);

        commandBuffer.endRendering();

        // Transition swap chain image: color attachment optimal -> present src
        transitionImageLayout(imageIndex, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
                              vk::AccessFlagBits2::eColorAttachmentWrite, {},
                              vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                              vk::PipelineStageFlagBits2::eBottomOfPipe);

        commandBuffer.end();
    }

    // Create Sync Objects //
    

    void Application::createSyncObjects()
    {
        assert(m_PresentCompleteSemaphores.empty() && m_RenderFinishedSemaphores.empty() && m_InFlightFences.empty());

        for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        {
            m_RenderFinishedSemaphores.emplace_back(m_Device, vk::SemaphoreCreateInfo());
        }

        for (int i = 0; i < kMaxFramesInFlight; i++)
        {
            m_PresentCompleteSemaphores.emplace_back(m_Device, vk::SemaphoreCreateInfo());
            m_InFlightFences.emplace_back(m_Device, vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
        }
    }

    // Draw Frame //
    void Application::drawFrame()
    {
        // m_InFlightFences, m_PresentCompleteSemaphores, and m_CommandBuffers are indexed by m_FrameIndex,
        // while m_RenderFinishedSemaphores is indexed by imageIndex.
        auto fenceResult = m_Device.waitForFences(*m_InFlightFences[m_FrameIndex], vk::True, UINT64_MAX);
        if (fenceResult != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to wait for fence!");
        }

        auto [result, imageIndex] =
            m_SwapChain.acquireNextImage(UINT64_MAX, *m_PresentCompleteSemaphores[m_FrameIndex], nullptr);

        if (result == vk::Result::eErrorOutOfDateKHR)
        {
            recreateSwapChain();
            return;
        }
        if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        m_Device.resetFences(*m_InFlightFences[m_FrameIndex]);

        m_CommandBuffers[m_FrameIndex].reset();
        recordCommandBuffer(imageIndex);

        vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        const vk::SubmitInfo submitInfo{.waitSemaphoreCount = 1,
                                        .pWaitSemaphores = &*m_PresentCompleteSemaphores[m_FrameIndex],
                                        .pWaitDstStageMask = &waitDestinationStageMask,
                                        .commandBufferCount = 1,
                                        .pCommandBuffers = &*m_CommandBuffers[m_FrameIndex],
                                        .signalSemaphoreCount = 1,
                                        .pSignalSemaphores = &*m_RenderFinishedSemaphores[imageIndex]};
        m_GraphicsQueue.submit(submitInfo, *m_InFlightFences[m_FrameIndex]);

        const vk::PresentInfoKHR presentInfoKHR{.waitSemaphoreCount = 1,
                                                .pWaitSemaphores = &*m_RenderFinishedSemaphores[imageIndex],
                                                .swapchainCount = 1,
                                                .pSwapchains = &*m_SwapChain,
                                                .pImageIndices = &imageIndex};
        result = m_GraphicsQueue.presentKHR(presentInfoKHR);

        if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR) ||
            m_FramebufferResized)
        {
            m_FramebufferResized = false;
            recreateSwapChain();
        }
        else
        {
            assert(result == vk::Result::eSuccess);
        }

        m_FrameIndex = (m_FrameIndex + 1) % kMaxFramesInFlight;
    }
    

    /******************************/
    // Swapchain Helper Functions //
    /******************************/

    uint32_t Application::chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
    {
        uint32_t minImageCount = std::max(3u, capabilities.minImageCount);
        if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < minImageCount)
        {
            minImageCount = capabilities.maxImageCount;
        }
        return minImageCount;
    }

    vk::SurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
    {
        for (const auto& format : availableFormats)
        {
            if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return format;
            }
        }
        return availableFormats[0];
    }

    vk::PresentModeKHR Application::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
    {
        for (const auto& mode : availablePresentModes)
        {
            if (mode == vk::PresentModeKHR::eMailbox)
            {
                return mode;
            }
        }
        return vk::PresentModeKHR::eFifo; // guaranteed to be available by the Vulkan spec
    }

    vk::Extent2D Application::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        int width, height;
        glfwGetFramebufferSize(m_Window, &width, &height);

        vk::Extent2D actualExtent{.width = static_cast<uint32_t>(width), .height = static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    // Transistion Image Layout
    void Application::transitionImageLayout(uint32_t imageIndex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                            vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
                                            vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask)
    {
        vk::ImageMemoryBarrier2 barrier{.srcStageMask = srcStageMask,
                                        .srcAccessMask = srcAccessMask,
                                        .dstStageMask = dstStageMask,
                                        .dstAccessMask = dstAccessMask,
                                        .oldLayout = oldLayout,
                                        .newLayout = newLayout,
                                        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                        .image = m_SwapChainImages[imageIndex],
                                        .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                                                             .baseMipLevel = 0,
                                                             .levelCount = 1,
                                                             .baseArrayLayer = 0,
                                                             .layerCount = 1}};

        vk::DependencyInfo dependencyInfo{.imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrier};

        m_CommandBuffers[m_FrameIndex].pipelineBarrier2(dependencyInfo);
    }

} // namespace Foxy

