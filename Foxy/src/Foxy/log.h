#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "fxpch.h"

// Core log macros
//#define FXC_TRACE(...)    ::Foxy::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define FXC_INFO(...)     ::Foxy::Log::GetCoreLogger()->info(__VA_ARGS__)
#define FXC_WARN(...)     ::Foxy::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define FXC_ERROR(...)    ::Foxy::Log::GetCoreLogger()->error(__VA_ARGS__)
#define FXC_CRITICAL(...) ::Foxy::Log::GetCoreLogger()->critical(__VA_ARGS__)
                                
// Client log macros            Foxy
#define FX_TRACE(...)         ::Foxy::Log::GetClientLogger()->trace(__VA_ARGS__)
#define FX_INFO(...)          ::Foxy::Log::GetClientLogger()->info(__VA_ARGS__)
#define FX_WARN(...)          ::Foxy::Log::GetClientLogger()->warn(__VA_ARGS__)  
#define FX_ERROR(...)         ::Foxy::Log::GetClientLogger()->error(__VA_ARGS__)
#define FX_CRITICAL(...)      ::Foxy::Log::GetClientLogger()->critical(__VA_ARGS__)

#ifdef NDEBUG
// Release Mode
#define FXC_TRACE(...) ::Foxy::Log::GetClientLogger()->trace(__VA_ARGS__)
#define FXC_INFO(...) ::Foxy::Log::GetClientLogger()->info(__VA_ARGS__)
#define FXC_WARN(...) ::Foxy::Log::GetClientLogger()->warn(__VA_ARGS__)
#else
 // Debug Mode
#define FXC_TRACE(...)
#define FXC_INFO(...)
#define FXC_WARN(...)
#endif

namespace Foxy
{

    class Log
    {
    public:
        static void Init_Logger();
        static void ShutDown_Logger();

        static std::shared_ptr<spdlog::logger>& GetCoreLogger()
        {
            return s_CoreLogger;
        }
        static std::shared_ptr<spdlog::logger>& GetClientLogger()
        {
            return s_ClientLogger;
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };

} // namespace Foxy

//template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
//inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
//{
//    return os << glm::to_string(vector);
//}
//
//template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
//inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
//{
//    return os << glm::to_string(matrix);
//}
//
//template <typename OStream, typename T, glm::qualifier Q>
//inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
//{
//    return os << glm::to_string(quaternion);
//}
