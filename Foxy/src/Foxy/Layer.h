//// Foxy/src/Foxy/Layer.h
//#pragma once
//
//namespace Foxy
//{
//
//    class Layer
//    {
//    public:
//        virtual ~Layer() = default;
//
//        virtual void OnAttach() {}
//        virtual void OnDetach() {}
//
//        virtual void OnUpdate(float ts) {}
//        virtual void OnUIRender() {}
//    };
//
//} // namespace Walnut

// Foxy/src/Foxy/Layer.h
#pragma once

#include "Application.h"
#include "fxpch.h"

namespace Foxy
{
    // Minimal render context handed to a Layer each frame - deliberately raw/raii
    // Vulkan for now, matching where the rest of the engine currently is.
    // (This is the seam that would eventually become nvrhi::IFramebuffer* instead,
    // if/when NVRHI actually takes over rendering - see Section 5 NVRHI notes.)
    struct FrameContext
    {
        VkCommandBuffer commandBuffer;
        //vk::CommandBuffer commandBuffer;
        vk::Extent2D extent;
        vk::raii::ImageView& targetImageView; // the swap chain image view for this frame
    };

    class Layer
    {
    public:
        virtual ~Layer() = default;

        virtual void OnAttach() {}                  // called once, after Vulkan/device/swapchain exist
        virtual void OnDetach() {}                  // called once, before device teardown
        virtual void OnUpdate(float ts) {}          // CPU-side per-frame logic (camera, input, etc.)
        virtual void OnRender(FrameContext& ctx) {} // GPU-side: bind pipeline, draw
    };
} // namespace Foxy