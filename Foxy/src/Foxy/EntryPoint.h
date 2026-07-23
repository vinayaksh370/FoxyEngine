// Foxy/src/Foxy/EntryPoint.h
//#pragma once
//
//extern Foxy::Application* Foxy::CreateApplication(int argc, char** argv);
//
//    namespace Foxy
//{
//    // ============================================================
//    // ACTIVE: simplest version — no restart loop, console toggle
//    // driven by the standard Debug/Release distinction (NDEBUG),
//    // matching kEnableValidationLayers' existing convention.
//    // No FOXY_DIST macro needed — CMake's WIN32_EXECUTABLE property
//    // (see FoxyApp/CMakeLists.txt) handles console vs windowed.
//    // ============================================================
//    int Main(int argc, char** argv)
//    {
//        Foxy::Application* app = Foxy::CreateApplication(argc, argv);
//        app->Run();
//        delete app;
//        return 0;
//    }
//}
//
//#ifdef FOXY_PLATFORM_WINDOWS
//#include <Windows.h>
//
//int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int)
//{
//    return Foxy::Main(__argc, __argv);
//}
//
//int main(int argc, char** argv)
//{
//    return Foxy::Main(argc, argv);
//}
//#endif

/*
// ============================================================
// OPTION A: Walnut-style — explicit FOXY_DIST macro instead of
// piggybacking on NDEBUG. Only worth this if you want a THIRD
// build tier later (Debug / Release / Dist) that's distinct from
// plain Release — e.g. Release still has asserts/logging, Dist
// strips everything. Skip unless you actually add that tier.
// ============================================================
namespace
{
    bool g_ApplicationRunning = true;
}

namespace Foxy
{
    int Main(int argc, char** argv)
    {
        while (g_ApplicationRunning)
        {
            Foxy::Application* app = Foxy::CreateApplication(argc, argv);
            app->Run();
            delete app;
        }
        return 0;
    }
}

#ifdef FOXY_PLATFORM_WINDOWS
    #ifdef FOXY_DIST
        #include <Windows.h>
        int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int)
        {
            return Foxy::Main(__argc, __argv);
        }
    #else
        int main(int argc, char** argv)
        {
            return Foxy::Main(argc, argv);
        }
    #endif
#endif
*/

/*
// ============================================================
// OPTION B: restart loop WITHOUT the FOXY_DIST split — combine
// with the ACTIVE block's NDEBUG-driven WIN32_EXECUTABLE approach
// if/when Application's teardown is proven safe to run twice in
// one process. Swap this in for the ACTIVE Main() above, keep the
// WinMain/main block from ACTIVE as-is.
// ============================================================
namespace
{
    bool g_ApplicationRunning = true;
}

namespace Foxy
{
    int Main(int argc, char** argv)
    {
        while (g_ApplicationRunning)
        {
            Foxy::Application* app = Foxy::CreateApplication(argc, argv);
            app->Run();
            delete app;
        }
        return 0;
    }
}
*/