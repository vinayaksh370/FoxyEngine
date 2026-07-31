// FoxyApp/src/main.cpp
#include "Foxy.h"

#include "Foxy/Application.h"
#include "Foxy/Log.h"

#include "fxpch.h"

int main()
{
    try
    {
        ////
        Foxy::Log::Init_Logger();
        FXC_INFO("[VULKAN] Initialization Started");

        FXC_TRACE("TRACE");
        FXC_INFO("INFO");
        FXC_WARN("WARN");
        FXC_ERROR("ERROR");
        FXC_CRITICAL("CRITICAL");

        /*int a = 5;
        FX_INFO("Hello var={0}", a);*/
        ////

        Foxy::ApplicationSpecification spec;
        spec.Name = "Foxy Engine";

        Foxy::Application app(spec);
        app.Run();

        // #ifdef _DEBUG
        //         deviceParams.enableDebugRuntime = true;
        //         deviceParams.enableNvrhiValidationLayer = true;
        // #endif2
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
