//  To be included in all 

#pragma once

#include <memory>
#include <optional>
#include <span>
#include <array>
#include <functional>
#include <deque>
#include <stdexcept>
#include <format>
#include <ranges>

#include <cstdint>   // For exact number types
#include <string>    // For text strings
#include <vector>    // For lists/arrays
#include <iostream>  // For printing messages
#include <algorithm> // For std::clamp
#include <fstream>   // For r/w files

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
//#include <vk_mem_alloc.h>


#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
//#define VK_CHECK(x)                                                                                                    \
//    do                                                                                                                 \
//    {                                                                                                                  \
//        VkResult err = x;                                                                                              \
//        if (err)                                                                                                       \
//        {                                                                                                              \
//            std::cerr << std::format("Detected Vulkan error: {}", string_VkResult(err)) << std::endl;                  \
//            abort();                                                                                                   \
//        }                                                                                                              \
//    } while (0)