#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace plt
{

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    VkSurfaceFormatKHR choose_format() const;

    VkPresentModeKHR choose_present_mode() const;

    VkExtent2D choose_extent(const VkExtent2D &extent) const;
};

} // namespace plt
