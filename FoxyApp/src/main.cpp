// FoxyApp/src/main.cpp
#include "Foxy/Application.h"

#include "Foxy/vk_types.h"

int main()
{
    try
    {
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
