// PreCompiled Header :: To be included everything

#pragma once

// ---- C++ standard library ----
#include <memory>           // smart pointers
#include <utility>          // std::move, std::pair, etc.
#include <optional>
#include <span>
#include <array>
#include <functional>
#include <deque>
#include <stdexcept>        // std::runtime_error
#include <format>           // std::format
#include <ranges>
#include <cassert>          // assert()
#include <unordered_map>
#include <unordered_set>

#include <cstdint>          // exact-width integer types (uint32_t etc.)
#include <cstring>          // strcmp() - used in isDeviceSuitable()'s extension matching
#include <string>           // std::string
#include <vector>           // std::vector
#include <iostream>         // std::cout / std::cerr
#include <algorithm>        // std::clamp
#include <fstream>          // file reading (readFile(), shader loading)
#include <sstream>          // std::stringstream, useful for building diagnostic strings

// ---- GLM (math library) ----
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

// ---- Raw Vulkan C API ----
// Safe to precompile: unlike vulkan_raii.hpp, this header has no dependency on
// project-defined config macros set immediately before inclusion.
#include <vulkan/vulkan.h>
//#include <vulkan/vulkan.hpp>
#include <vulkan/vk_enum_string_helper.h> // string_VkResult() etc., used by VK_CHECK

// ---- NVRHI ----
// Self-contained, no required pre-include #defines for a static (non-shared) build.
#include <nvrhi/nvrhi.h>
#include <nvrhi/vulkan.h>
#include <nvrhi/validation.h>
