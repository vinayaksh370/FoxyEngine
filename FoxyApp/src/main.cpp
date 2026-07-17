// FoxyApp/src/main.cpp
#include "Foxy/Application.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        Foxy::Application app;
        app.Run();
        
//#ifdef _DEBUG
//        deviceParams.enableDebugRuntime = true;
//        deviceParams.enableNvrhiValidationLayer = true;
//#endif

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
