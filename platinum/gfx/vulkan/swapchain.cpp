#include <algorithm>
#include <cstdint>
#include <limits>
#include <platinum/gfx/vulkan/swapchain.hpp>
#include <platinum/gfx/vulkan/vulkan.hpp>
namespace plt
{

Result<std::unique_ptr<Swapchain>> Swapchain::create(const SwapchainSupportDetails &details, const Device &device, const Surface &surface)
{
    std::unique_ptr<Swapchain> swapchain = std::make_unique<Swapchain>(device, surface);
    swapchain->_format = details.choose_format();
    swapchain->_present_mode = details.choose_present_mode();
    swapchain->_extent = details.choose_extent(surface.size());

    uint32_t image_count = details.capabilities.minImageCount + 1;

    if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount)
    {
        image_count = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface.handle(),

        .minImageCount = image_count,
        .imageFormat = swapchain->_format.format,
        .imageColorSpace = swapchain->_format.colorSpace,
        .imageExtent = swapchain->_extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

        .queueFamilyIndexCount = 0,

        .preTransform = details.capabilities.currentTransform,

        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = swapchain->_present_mode,
        .clipped = VK_TRUE,

        .oldSwapchain = VK_NULL_HANDLE, // for later
    };

    uint32_t queue_family_indices[] = {uint32_t(device.index().presentFamily), uint32_t(device.index().graphicsFamily)};
    if (device.index().presentFamily != device.index().graphicsFamily)
    {

        // Images can be used across multiple queue families without explicit
        // ownership transfers.
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queue_family_indices;

        warn$("using sharing_mode_concurrent, it may be slower than exclusive");
    }
    else
    {
        // An image is owned by one queue family at a time and ownership
        // must be explicitly transferred before using it in another queue
        // family. This option offers the best performance.
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    vk_try$(vkCreateSwapchainKHR(device.handle(), &createInfo, nullptr, &swapchain->_swapchain));

    vk_try$(vkGetSwapchainImagesKHR(device.handle(), swapchain->_swapchain, &image_count, nullptr));
    swapchain->_images.resize(image_count);
    vk_try$(vkGetSwapchainImagesKHR(device.handle(), swapchain->_swapchain, &image_count, swapchain->_images.data()));

    return Result<std::unique_ptr<Swapchain>>::ok(std::move(swapchain));
}

void Swapchain::destroy()
{
    vkDestroySwapchainKHR(_device.handle(), _swapchain, nullptr);
}

Result<Swapchain::ImageID> Swapchain::acquire_next_image(Semaphore &semaphore_on_complete)
{
    uint32_t image_index;
    VkResult result = (vkAcquireNextImageKHR(_device.handle(), _swapchain, std::numeric_limits<uint64_t>::max(), semaphore_on_complete.handle(), VK_NULL_HANDLE, &image_index));

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR)
    {
        vk_try$(result);
    }

    return success((Swapchain::ImageID){
        .id = image_index,
        .out_of_date_swapchain = result == VK_ERROR_OUT_OF_DATE_KHR,
    });
}

Result<> Swapchain::create_image_view()
{
    _image_views.resize(_images.size());
    for (size_t i = 0; i < _images.size(); i++)
    {
        VkImageViewCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = _images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = _format.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        vk_try$(vkCreateImageView(_device.handle(), &createInfo, nullptr, &_image_views[i]));
    }

    return Result<>::ok({});
}

void Swapchain::destroy_image_view()
{
    for (auto &image_view : _image_views)
    {
        vkDestroyImageView(_device.handle(), image_view, nullptr);
    }
}
Result<Swapchain::ImageID> Swapchain::present(uint32_t image_index, Semaphore &signaled_semaphore)
{
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &signaled_semaphore.handle(),
        .swapchainCount = 1,
        .pSwapchains = &_swapchain,
        .pImageIndices = &image_index,
    };
    VkResult result = vkQueuePresentKHR(_device.present_queue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        return success((Swapchain::ImageID){
            .id = image_index,
            .out_of_date_swapchain = true,
        });
    }
    else
    {
        vk_try$(result);
    }
    return success((Swapchain::ImageID){
        .id = image_index,
        .out_of_date_swapchain = false,
    });
}
} // namespace plt
