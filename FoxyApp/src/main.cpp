// FoxyApp/src/main.cpp
#include "Foxy.h"

#include "Foxy/Application.h"
#include "Foxy/Log.h"

#include "fxpch.h"

int main()
{
    try
    {
        Foxy::ApplicationSpecification spec;
        spec.Name = "Foxy Engine";

        Foxy::Application app(spec);
        app.Run();
    }
    catch (const std::exception& e)
    {
        /*std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;*/
        
        FXC_CRITICAL("Unhandled exception: {}", e.what());
        return EXIT_FAILURE;
        
    }

    return EXIT_SUCCESS;
}
