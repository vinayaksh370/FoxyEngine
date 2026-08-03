// FoxyApp/src/main.cpp
#include "Foxy.h"

#include "Foxy/Application.h"
#include "Foxy/Log.h"

#include "fxpch.h"

#include "ExampleLayer.h"

class PINKLAYER : public Foxy::Layer
{
public:
    PINKLAYER() : Layer("Example") {}

    void OnAttach() override
    {
        m_CommandList = Foxy::Application::GetNvrhiDevice()->createCommandList();
        FX_INFO("PINKLAYER attached.");
    }

    void OnRender(nvrhi::IFramebuffer* framebuffer) override
    {
        m_CommandList->open();

        nvrhi::utils::ClearColorAttachment(m_CommandList, framebuffer, 0, nvrhi::Color(1.0f, 0.0f, 0.7f, 1.0f));

        // Pipeline bind/draw goes here once we confirm this minimal version fixes the semaphore errors.

        m_CommandList->close();
        Foxy::Application::GetNvrhiDevice()->executeCommandList(m_CommandList);
    }

private:
    nvrhi::CommandListHandle m_CommandList;
};

int main()
{
    /*try
    {*/
        Foxy::ApplicationSpecification spec;
        spec.Name = "Foxy Engine";

        Foxy::Application app(spec);

        app.PushLayer<PINKLAYER>();
        app.PushLayer<ExampleLayer>();
        app.Run();
    //}
    //catch (const std::exception& e)
    //{
    //    /*std::cerr << e.what() << std::endl;
    //    return EXIT_FAILURE;*/
    //    
    //    FXC_CRITICAL("Unhandled exception: {}", e.what());
    //    return EXIT_FAILURE;
    //    
    //}

    //return EXIT_SUCCESS;
}
