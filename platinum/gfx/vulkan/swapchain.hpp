#pragma once
#include <platinum/gfx/vulkan/sync.hpp>
#include <platinum/utility/result.hpp>
#include <platinum/utility/trait.hpp>
#include <vector>
#include <vulkan/vulkan.h>

#include <platinum/gfx/vulkan/logical_device.hpp>
#include <platinum/gfx/vulkan/swapchain_support.hpp>
namespace plt
{
class ImageView
{
};

class Swapchain
{
public:
    Swapchain(const Device &device, const Surface &surface) : _device(device), _surface(surface), _extent(surface.size()){};

    const std::vector<VkImage> &images() const { return _images; }
    const std::vector<VkImageView> &image_view() const { return _image_views; }

    Result<> create_image_view();

    void destroy_image_view();

    static Result<std::unique_ptr<Swapchain>> create(const SwapchainSupportDetails &details, const Device &device, const Surface &surface);

    VkExtent2D extent() const { return _extent; }

    void destroy();

    VkSurfaceFormatKHR format() const { return _format; }

    struct ImageID
    {
        uint32_t id;
        bool out_of_date_swapchain;
    };
    Result<Swapchain::ImageID> acquire_next_image(Semaphore &semaphore_on_complete);

    Result<Swapchain::ImageID> present(uint32_t image_index, Semaphore &signaled_semaphore);

protected:
    const Device &_device;
    const Surface &_surface;
    VkExtent2D _extent;
    VkSurfaceFormatKHR _format;
    VkPresentModeKHR _present_mode;
    SwapchainSupportDetails _support;
    VkSwapchainKHR _swapchain;
    std::vector<VkImage> _images;
    std::vector<VkImageView> _image_views;
    friend class Device;
};
} // namespace plt
