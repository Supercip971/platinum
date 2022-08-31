#pragma once

#include <platinum/gfx/render.hpp>
#include <platinum/gfx/vulkan/surface.hpp>
#include <vulkan/vulkan.h>

#include <platinum/gfx/vulkan/physical_device.hpp>

namespace plt
{

using ReqExtensions = std::vector<const char *>;
using ReqLayers = std::vector<const char *>;

class Instance
{
public:
    using InstancePtr = std::unique_ptr<Instance>;

    VkInstance instance;

    Result<std::unique_ptr<plt::PhysicalDevice>> pick_physical_device();

    Result<std::unique_ptr<plt::Surface>> create_surface(plt::Window &target_surface);

    void destroy_surface(plt::Surface &surface);

    static Result<int> check_extensions(const plt::ReqExtensions &required_exts);

    static Result<int> check_layers(const plt::ReqLayers &required_layers);

    static Result<InstancePtr> create(const plt::ReqExtensions &required_exts, const plt::ReqLayers &required_layers);

    static void destroy(Instance &instance);
};

} // namespace plt
