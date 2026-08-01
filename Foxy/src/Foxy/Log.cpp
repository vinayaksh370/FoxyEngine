#include "Log.h"

namespace Foxy
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init_Logger()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("FOXY");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}

	void Log::ShutDown_Logger()
	{
		// TODO:
	}

	// The Debug Callbacks For Vulkan and NVRHI .... Needed in Application.cpp
	//class NvrhiMessageCallback : public nvrhi::IMessageCallback
	//{
	//public:
	//	void message(nvrhi::MessageSeverity severity, const char* messageText) override
	//	{
	//		switch (severity)
	//		{
	//		case nvrhi::MessageSeverity::Fatal:
	//			FXC_CRITICAL("NVRHI: {}", messageText);
	//			break;
	//		case nvrhi::MessageSeverity::Error:
	//			FXC_ERROR("NVRHI: {}", messageText);
	//			break;
	//		case nvrhi::MessageSeverity::Warning:
	//			FXC_WARN("NVRHI: {}", messageText);
	//			break;
 //           case nvrhi::MessageSeverity::Info:
 //               FXC_INFO("NVRHI: {}", messageText);
 //               break;
	//		default: // 
	//			FXC_TRACE("NVRHI: {}", messageText);
	//			break;
	//		}
	//	}
	//};

	//// Static function for Vulkan's callback
	//static VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanDebugCallback(
	//	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, vk::DebugUtilsMessageTypeFlagsEXT messageType,
	//	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	//{
	//	if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
	//	{
	//		FXC_ERROR("Vulkan: {}", pCallbackData->pMessage);
	//	}
	//	else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
	//	{
	//		FXC_WARN("Vulkan: {}", pCallbackData->pMessage);
	//	}
	//	else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
	//	{
	//		FXC_INFO("Vulkan: {}", pCallbackData->pMessage); // Milestone/Setup Info
	//	}
	//	else
	//	{
	//		FXC_TRACE("Vulkan [Verbose]: {}", pCallbackData->pMessage); // API spam
	//	}
	//	return vk::False;
	//}
}