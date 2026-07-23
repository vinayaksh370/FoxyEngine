#pragma once

#if DONUT_WITH_VULKAN
#include <nvrhi/vulkan.h>
#endif

#define GLFW_INCLUDE_NONE // Do not include any OpenGL headers
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.h>
#include <nvrhi/nvrhi.h>

// #include <donut/core/log.h>

#include <list>
#include <functional>
#include <string>

namespace Foxy
{
    struct DefaultMessageCallback : public nvrhi::IMessageCallback
    {
        static DefaultMessageCallback& GetInstance();

        void message(nvrhi::MessageSeverity severity, const char* messageText) override;
    };

    struct InstanceParameters
    {
        bool enableDebugRuntime = false;
        bool headlessDevice = false;

#if DONUT_WITH_VULKAN
        std::vector<std::string> requiredVulkanInstanceExtensions;
        std::vector<std::string> requiredVulkanLayers;
        std::vector<std::string> optionalVulkanInstanceExtensions;
        std::vector<std::string> optionalVulkanLayers;
#endif
    };

    struct DeviceCreationParameters : public InstanceParameters
    {
        bool startMaximized = false;
        bool startFullscreen = false;
        bool allowModeSwitch = true;
        int windowPosX = -1; // -1 means use default placement
        int windowPosY = -1;
        uint32_t backBufferWidth = 1280;
        uint32_t backBufferHeight = 720;
        uint32_t refreshRate = 0;
        uint32_t swapChainBufferCount = 3;
        nvrhi::Format swapChainFormat = nvrhi::Format::SRGBA8_UNORM;
        uint32_t swapChainSampleCount = 1;
        uint32_t swapChainSampleQuality = 0;
        uint32_t maxFramesInFlight = 2;
        bool enableNvrhiValidationLayer = false;
        bool vsyncEnabled = false;
        bool enableRayTracingExtensions = false;
        bool enableComputeQueue = false;
        bool enableCopyQueue = false;

        // Severity of the information log messages from the device manager, like the device name or enabled extensions.
        //log::Severity infoLogSeverity = log::Severity::Info;

        // Index of the physical device on which to initialize the device.
        // Negative values mean automatic detection.
        // The order of indices matches that returned by DeviceManager::EnumerateAdapters.
        //int adapterIndex = -1;

        // set to true to enable DPI scale factors to be computed per monitor
        // this will keep the on-screen window size in pixels constant
        //
        // if set to false, the DPI scale factors will be constant but the system
        // may scale the contents of the window based on DPI
        //
        // note that the backbuffer size is never updated automatically; if the app
        // wishes to scale up rendering based on DPI, then it must set this to true
        // and respond to DPI scale factor changes by resizing the backbuffer explicitly
        //bool enablePerMonitorDPI = false;

        // #if DONUT_WITH_VULKAN
        std::vector<std::string> requiredVulkanDeviceExtensions;
        std::vector<std::string> optionalVulkanDeviceExtensions;
        std::vector<size_t> ignoredVulkanValidationMessageLocations;
        //std::function<void(VkDeviceCreateInfo&)> deviceCreateInfoCallback;
        // #endif
    };

    class IRenderPass;

    struct AdapterInfo
    {
        std::string name;
        uint32_t vendorID = 0;
        uint32_t deviceID = 0;
        uint64_t dedicatedVideoMemory = 0;
#if DONUT_WITH_VULKAN
        VkPhysicalDevice vkPhysicalDevice = nullptr;
#endif
    };

    class DeviceManager
    {
    public:
        static DeviceManager* Create(nvrhi::GraphicsAPI api);

        bool CreateHeadlessDevice(const DeviceCreationParameters& params);
        bool CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, const char* windowTitle);

        // Initializes Vulkan instance.
        // Calling CreateInstance() is required before EnumerateAdapters(), but optional if you don't use
        // EnumerateAdapters(). Note: if you call CreateInstance before Create*Device*(), the values in
        // InstanceParameters must match those in DeviceCreationParameters passed to the device call.
        bool CreateInstance(const InstanceParameters& params);

        // Enumerates physical devices present in the system.
        // Note: a call to CreateInstance() or Create*Device*() is required before EnumerateAdapters().
        virtual bool EnumerateAdapters(std::vector<AdapterInfo>& outAdapters) = 0;

        void AddRenderPassToFront(IRenderPass* pController);
        void AddRenderPassToBack(IRenderPass* pController);
        void RemoveRenderPass(IRenderPass* pController);

        void RunMessageLoop();

        // returns the size of the window in screen coordinates
        void GetWindowDimensions(int& width, int& height);
        // returns the screen coordinate to pixel coordinate scale factor
        void GetDPIScaleInfo(float& x, float& y) const
        {
            x = m_DPIScaleFactorX;
            y = m_DPIScaleFactorY;
        }

    protected:
        bool m_windowVisible = false;

        DeviceCreationParameters m_DeviceParams;
        GLFWwindow* m_Window = nullptr;
        bool m_EnableRenderDuringWindowMovement = false;
        bool m_IsNvidia = false;
        std::list<IRenderPass*> m_vRenderPasses;
        double m_PreviousFrameTimestamp = 0.0;
        float m_DPIScaleFactorX = 1.f;
        float m_DPIScaleFactorY = 1.f;
        bool m_RequestedVSync = false;
        bool m_InstanceCreated = false;

        double m_AverageFrameTime = 0.0;
        double m_AverageTimeUpdateInterval = 0.5;
        double m_FrameTimeSum = 0.0;
        int m_NumberOfAccumulatedFrames = 0;

        uint32_t m_FrameIndex = 0;

        std::vector<nvrhi::FramebufferHandle> m_SwapChainFramebuffers;

        DeviceManager() = default;

        void UpdateWindowSize();

        void BackBufferResizing();
        void BackBufferResized();

        void Animate(double elapsedTime);
        void Render();
        void UpdateAverageFrameTime(double elapsedTime);
        void AnimateRenderPresent();

        // device-specific methods
        virtual bool CreateInstanceInternal() = 0;
        virtual bool CreateDevice() = 0;
        virtual bool CreateSwapChain() = 0;
        virtual void DestroyDeviceAndSwapChain() = 0;
        virtual void ResizeSwapChain() = 0;
        virtual void BeginFrame() = 0;
        virtual void Present() = 0;

    public:
        [[nodiscard]] virtual nvrhi::IDevice* GetDevice() const = 0;
        [[nodiscard]] virtual const char* GetRendererString() const = 0;
        [[nodiscard]] virtual nvrhi::GraphicsAPI GetGraphicsAPI() const = 0;

        const DeviceCreationParameters& GetDeviceParams();
        [[nodiscard]] double GetAverageFrameTimeSeconds() const
        {
            return m_AverageFrameTime;
        }
        [[nodiscard]] double GetPreviousFrameTimestamp() const
        {
            return m_PreviousFrameTimestamp;
        }
        void SetFrameTimeUpdateInterval(double seconds)
        {
            m_AverageTimeUpdateInterval = seconds;
        }
        [[nodiscard]] bool IsVsyncEnabled() const
        {
            return m_DeviceParams.vsyncEnabled;
        }
        virtual void SetVsyncEnabled(bool enabled)
        {
            m_RequestedVSync = enabled; /* will be processed later */
        }
        virtual void ReportLiveObjects() {}
        void SetEnableRenderDuringWindowMovement(bool val)
        {
            m_EnableRenderDuringWindowMovement = val;
        }

        // these are public in order to be called from the GLFW callback functions
        void WindowCloseCallback() {}
        void WindowIconifyCallback(int iconified) {}
        void WindowFocusCallback(int focused) {}
        void WindowRefreshCallback() {}
        void WindowPosCallback(int xpos, int ypos);

        void KeyboardUpdate(int key, int scancode, int action, int mods);
        void KeyboardCharInput(unsigned int unicode, int mods);
        void MousePosUpdate(double xpos, double ypos);
        void MouseButtonUpdate(int button, int action, int mods);
        void MouseScrollUpdate(double xoffset, double yoffset);

        [[nodiscard]] GLFWwindow* GetWindow() const
        {
            return m_Window;
        }
        [[nodiscard]] uint32_t GetFrameIndex() const
        {
            return m_FrameIndex;
        }

        virtual nvrhi::ITexture* GetCurrentBackBuffer() = 0;
        virtual nvrhi::ITexture* GetBackBuffer(uint32_t index) = 0;
        virtual uint32_t GetCurrentBackBufferIndex() = 0;
        virtual uint32_t GetBackBufferCount() = 0;
        nvrhi::IFramebuffer* GetCurrentFramebuffer();
        nvrhi::IFramebuffer* GetFramebuffer(uint32_t index);

        virtual void Shutdown();
        virtual ~DeviceManager() = default;

        void SetWindowTitle(const char* title);
        void SetInformativeWindowTitle(const char* applicationName, const char* extraInfo = nullptr);

        virtual bool IsVulkanInstanceExtensionEnabled(const char* extensionName) const
        {
            return false;
        }
        virtual bool IsVulkanDeviceExtensionEnabled(const char* extensionName) const
        {
            return false;
        }
        virtual bool IsVulkanLayerEnabled(const char* layerName) const
        {
            return false;
        }
        virtual void GetEnabledVulkanInstanceExtensions(std::vector<std::string>& extensions) const {}
        virtual void GetEnabledVulkanDeviceExtensions(std::vector<std::string>& extensions) const {}
        virtual void GetEnabledVulkanLayers(std::vector<std::string>& layers) const {}

        struct PipelineCallbacks
        {
            std::function<void(DeviceManager&)> beforeFrame = nullptr;
            std::function<void(DeviceManager&)> beforeAnimate = nullptr;
            std::function<void(DeviceManager&)> afterAnimate = nullptr;
            std::function<void(DeviceManager&)> beforeRender = nullptr;
            std::function<void(DeviceManager&)> afterRender = nullptr;
            std::function<void(DeviceManager&)> beforePresent = nullptr;
            std::function<void(DeviceManager&)> afterPresent = nullptr;
        } m_callbacks;

    private:
        static DeviceManager* CreateVK();

        std::string m_WindowTitle;
    };

    class IRenderPass
    {
    private:
        DeviceManager* m_DeviceManager;

    public:
        explicit IRenderPass(DeviceManager* deviceManager) : m_DeviceManager(deviceManager) {}

        virtual ~IRenderPass() = default;

        virtual void Render(nvrhi::IFramebuffer* framebuffer) {}
        virtual void Animate(float fElapsedTimeSeconds) {}
        virtual void BackBufferResizing() {}
        virtual void BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount) {}

        // all of these pass in GLFW constants as arguments
        // see http://www.glfw.org/docs/latest/input.html
        // return value is true if the event was consumed by this render pass, false if it should be passed on
        virtual bool KeyboardUpdate(int key, int scancode, int action, int mods)
        {
            return false;
        }
        virtual bool KeyboardCharInput(unsigned int unicode, int mods)
        {
            return false;
        }
        virtual bool MousePosUpdate(double xpos, double ypos)
        {
            return false;
        }
        virtual bool MouseScrollUpdate(double xoffset, double yoffset)
        {
            return false;
        }
        virtual bool MouseButtonUpdate(int button, int action, int mods)
        {
            return false;
        }
        virtual bool JoystickButtonUpdate(int button, bool pressed)
        {
            return false;
        }
        virtual bool JoystickAxisUpdate(int axis, float value)
        {
            return false;
        }

        [[nodiscard]] DeviceManager* GetDeviceManager() const
        {
            return m_DeviceManager;
        }
        [[nodiscard]] nvrhi::IDevice* GetDevice() const
        {
            return m_DeviceManager->GetDevice();
        }
        [[nodiscard]] uint32_t GetFrameIndex() const
        {
            return m_DeviceManager->GetFrameIndex();
        }
    };
} // namespace donut::app
