# Foxy Engine

A personal C++20 Vulkan rendering engine, built by following the official
[Vulkan Tutorial](https://docs.vulkan.org/tutorial) chapter-by-chapter — but
translated by hand into the **raw C Vulkan API** instead of the tutorial's
Vulkan-Hpp RAII style. The goal is to learn Vulkan by writing every explicit
call myself, not to hide it behind wrapper sugar.

## Plan

- Instance → physical device → logical device → surface → swapchain: raw
  Vulkan, permanently.
- Once a working `VkDevice` + queues exist, everything downstream (pipelines,
  buffers, textures, draws, ray tracing) hands off to
  [NVRHI](https://github.com/NVIDIA-RTX/NVRHI).

## Project structure

```
FoxyEngineWorkspace/
├── Foxy/          # the engine (static library)
├── FoxyApp/       # runnable executable that uses Foxy
├── vendor/        # glfw, glm, imgui.cmake
└── nvrhi/         # third-party NVRHI library
```

## Building

```bash
cmake -B build -S .
cmake --build build
```

## Status

Following the tutorial chapter-by-chapter. Currently through **Physical
Devices and Queue Families**; Logical Device and Queues is next.
