#pragma once
#include <memory>
#include <platinum/gfx/vulkan/surface.hpp>
#include <platinum/utility/result.hpp>
#include <platinum/utility/trait.hpp>
#include <vector>
#include <vulkan/vulkan.h>

#include <platinum/gfx/vulkan/logical_device.hpp>
#include <platinum/gfx/vulkan/swapchain_support.hpp>

namespace plt
{

using VulkanQueueFamilies = std::vector<VkQueueFamilyProperties>;
using VulkanExtensions = std::vector<const char *>;
class PhysicalDevice
{
    VkPhysicalDevice _physical_device;

public:
    PhysicalDevice() : _physical_device(nullptr){};

    PhysicalDevice(VkPhysicalDevice physical_device) : _physical_device(physical_device)
    {
    }
    constexpr static VulkanExtensions get_required_extension()
    {
        return {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };
    }

    bool check_device_extension(const VulkanExtensions &extensions) const;

    bool is_suitable(Surface &surface) const;

    // used to pick, wich gpu is best to take
    int suitability_score() const;

    void dump() const;

    Result<VulkanQueueFamilyIndex> pick_queue_family_index(Surface &surface) const;

    Result<std::unique_ptr<Device>> create_logical_device(Surface &associated_surface);

    // we should not cache this (features/properties) because the structure is 1044 bytes long
    // and moving around/using the physical device becomes expansive for something
    // we only need during the initialization.
    inline VkPhysicalDeviceFeatures features() const
    {
        VkPhysicalDeviceFeatures feat;
        vkGetPhysicalDeviceFeatures(_physical_device, &feat);
        return feat;
    };

    inline VkPhysicalDeviceProperties properties() const
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(_physical_device, &props);
        return props;
    };

    inline VulkanQueueFamilies queue_families() const
    {
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queue_family_count, nullptr);
        VulkanQueueFamilies queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queue_family_count, queue_families.data());
        return queue_families;
    };

    SwapchainSupportDetails query_swapchain_support(Surface &surface) const;
};
}; // namespace plt
