#pragma once
#include <platinum/gfx/vulkan/surface.hpp>
#include <platinum/utility/result.hpp>
#include <platinum/utility/trait.hpp>
#include <vulkan/vulkan.h>

namespace plt
{

struct VulkanQueueFamilyIndex
{
    int graphicsFamily;
    int presentFamily;
};

class Device : public plt::NoCopy
{

public:
    Device(){};
    Device(Device &&other) = default;

    Device &operator=(Device &&other) = default;

    void destroy();

    void wait_idle();

    friend class PhysicalDevice;

    VulkanQueueFamilyIndex index() const { return _index; }

    const VkDevice &handle() const { return _device; }

    const VkQueue &graphics_queue() const { return _queue; }

    const VkQueue &present_queue() const { return _presentation_queue; }

protected:
    VkDevice _device;
    // maybe abstract that later
    VkQueue _queue;
    VkQueue _presentation_queue;
    VulkanQueueFamilyIndex _index;

    static Device create(VkDevice device, VulkanQueueFamilyIndex queueFamilyIndex)
    {
        Device dev;
        dev._device = device;
        dev._index = queueFamilyIndex;

        vkGetDeviceQueue(device, queueFamilyIndex.presentFamily, 0, &dev._presentation_queue);
        vkGetDeviceQueue(device, queueFamilyIndex.graphicsFamily, 0, &dev._queue);
        return dev;
    };
};
} // namespace plt
