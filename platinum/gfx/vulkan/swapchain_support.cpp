#include <algorithm>
#include <cstdint>
#include <limits>
#include <platinum/gfx/vulkan/swapchain.hpp>
#include <platinum/log/log.hpp>

namespace plt
{
VkSurfaceFormatKHR SwapchainSupportDetails::choose_format() const
{
    for (const auto &availableFormat : formats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    warn$("Wasn't able to select the default format");
    for (const auto &availableFormat : formats)
    {
        debug$("format: {} space: {}", int32_t(availableFormat.format), int32_t(availableFormat.colorSpace));
    }

    return formats[0];
}

VkPresentModeKHR SwapchainSupportDetails::choose_present_mode() const
{

    for (const auto &availablePresentMode : presentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapchainSupportDetails::choose_extent(const VkExtent2D &extent) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    VkExtent2D actual_extent = extent;

    actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actual_extent;
}
} // namespace plt
