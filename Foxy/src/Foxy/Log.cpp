#include "Log.h"

namespace Foxy
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init_Logger()
	{
		spdlog::set_pattern("%^[%T] [%l] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("FOXY");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}

	void Log::ShutDown_Logger()
	{
        spdlog::shutdown();
        s_ClientLogger.reset();
        s_CoreLogger.reset();
	}
}