// Foxy/src/Foxy/Application.h

#pragma once

#include <GLFW/glfw3.h>

#include "fxpch.h"
#include "Layer.h"

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
        ~Application();

        void Run();

        // Template for pushing layers to the stack [16, 18]
        template <typename T, typename... Args> 
        std::shared_ptr<T> PushLayer(Args&&... args)
        {
            static_assert(std::derived_from<T, Layer>, "T must derive from Layer");
            auto layer = std::make_shared<T>(std::forward<Args>(args)...);
            m_LayerStack.emplace_back(layer);
            //layer->OnAttach();
            return layer;
        }

        // Use when a layer is pre-constructed
        void PushLayer(std::shared_ptr<Layer> layer)
        {
            m_LayerStack.emplace_back(std::move(layer));
            m_LayerStack.back()->OnAttach();
        }

        std::vector<std::shared_ptr<Layer>>& GetLayerStack()
        {
            return m_LayerStack;
        }

        static Application& GetApp();

        GLFWwindow* GetWindow();

        float GetTime();

        static nvrhi::IDevice* GetNvrhiDevice();
        static nvrhi::GraphicsPipelineHandle GetNvrhiGraphicsPipeline();


    private:
        // App Lifecycle Functions - bodies live in Application.cpp, operating on
        // file-scope statics. None of these need Vulkan/GLFW/NVRHI types in 
        // own signature, so Application.h never needs to see those headers at all.
        void initWindow(); // Create the GLFW window
        void initVulkan(); // Set up Vulkan rendering
        void mainLoop();   // The main game/draw loop
        void cleanup();    // Clean up when closing

    private:
        ApplicationSpecification m_AppSpec;

        std::vector<std::shared_ptr<Layer>> m_LayerStack;

        float m_TimeStep = 0.0f;
        float m_FrameTime = 0.0f;
        float m_LastFrameTime = 0.0f; 
    };
} // namespace Foxy

