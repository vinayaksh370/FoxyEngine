// Foxy/src/Foxy/Layer.h
#pragma once

#include "Application.h"
#include "fxpch.h"

// Foxy/src/Foxy/Layer.h
namespace Foxy
{
    class Layer
    {
    public:
        Layer(const std::string& name = "Layer") : m_DebugName(name) {}
        virtual ~Layer() = default;

        virtual void OnAttach() {}            // Called when layer is pushed to the stack
        virtual void OnDetach() {}            // Called when layer is removed

        virtual void OnUpdate(float ts) {}    // Logic/Physics updates
        //virtual void OnRender() {}            // Main GAPI (NVRHI) rendering [8]

        virtual void OnRender(nvrhi::IFramebuffer* framebuffer) {}

        //virtual void OnUIRender() {}          // Dear ImGui rendering
        //virtual void OnEvent(Event& event) {} // Input/OS events 

        const std::string& GetName() const
        {
            return m_DebugName;
        }

    protected:
        std::string m_DebugName;
    };
} // namespace Foxy