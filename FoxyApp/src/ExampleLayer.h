// FoxyApp/src/ExampleLayer.h
#pragma once

#include "Foxy/Layer.h"
#include "Foxy/Application.h"
#include "Foxy/Log.h"

#include <nvrhi/utils.h>

class ExampleLayer : public Foxy::Layer
{
public:
    ExampleLayer() : Layer("Example") {}

    void OnAttach() override
    {
        m_CommandList = Foxy::Application::GetNvrhiDevice()->createCommandList();
        FX_INFO("ExampleLayer attached.");
    }

    void OnRender(nvrhi::IFramebuffer* framebuffer) override
    {
        m_CommandList->open();

        nvrhi::utils::ClearColorAttachment(m_CommandList, framebuffer, 0, nvrhi::Color(0.0f, 1.0f, 0.0f, 0.5f));

        // Pipeline bind/draw goes here once we confirm this minimal version fixes the semaphore errors.

        m_CommandList->close();
        Foxy::Application::GetNvrhiDevice()->executeCommandList(m_CommandList);
    }

private:
    nvrhi::CommandListHandle m_CommandList;
};