// FoxyApp/src/main.cpp
#include "Foxy.h"

#include "Foxy/Application.h"
#include "Foxy/Log.h"

#include "Foxy/vk_types.h"

int main()
{
    try
    {
        ////
        Foxy::Log::Init();
        FX_CORE_INFO("[VULKAN] Initialization Started");

        FX_CORE_TRACE("TRACE");
        FX_CORE_INFO("INFO");
        FX_CORE_WARN("WARN");
        FX_CORE_ERROR("ERROR");
        FX_CORE_CRITICAL("CRITICAL");

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
