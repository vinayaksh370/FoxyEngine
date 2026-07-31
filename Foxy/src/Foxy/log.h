#pragma once

// 1. Set the active level based on build configuration
#ifdef NDEBUG
// In Release mode, strip everything below INFO (Trace and Debug are removed)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR
#else
// In Debug mode, allow everything down to TRACE
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "fxpch.h"

// 2. Define Core Logger Macros
// These use SPDLOG_LOGGER_... which strip calls at compile-time if active level is too low
#define FXC_TRACE(...)    SPDLOG_LOGGER_TRACE(::Foxy::Log::GetCoreLogger(), __VA_ARGS__)
#define FXC_INFO(...)     SPDLOG_LOGGER_INFO(::Foxy::Log::GetCoreLogger(), __VA_ARGS__)
#define FXC_WARN(...)     SPDLOG_LOGGER_WARN(::Foxy::Log::GetCoreLogger(), __VA_ARGS__)
#define FXC_ERROR(...)    SPDLOG_LOGGER_ERROR(::Foxy::Log::GetCoreLogger(), __VA_ARGS__)
#define FXC_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::Foxy::Log::GetCoreLogger(), __VA_ARGS__)

// 3. Define Client Logger Macros
#define FX_TRACE(...)     SPDLOG_LOGGER_TRACE(::Foxy::Log::GetClientLogger(), __VA_ARGS__)
#define FX_INFO(...)      SPDLOG_LOGGER_INFO(::Foxy::Log::GetClientLogger(), __VA_ARGS__)
#define FX_WARN(...)      SPDLOG_LOGGER_WARN(::Foxy::Log::GetClientLogger(), __VA_ARGS__)
#define FX_ERROR(...)     SPDLOG_LOGGER_ERROR(::Foxy::Log::GetClientLogger(), __VA_ARGS__)
#define FX_CRITICAL(...)  SPDLOG_LOGGER_CRITICAL(::Foxy::Log::GetClientLogger(), __VA_ARGS__)

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
