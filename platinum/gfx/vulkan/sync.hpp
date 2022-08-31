#pragma once
#include <vulkan/vulkan.h>

#include <platinum/gfx/vulkan/logical_device.hpp>

namespace plt
{

class Semaphore
{
public:
    Semaphore(const Device &device, VkSemaphore semaphore) : _device(device), _semaphore(semaphore) {}
    void destroy()
    {
        vkDestroySemaphore(_device.handle(), _semaphore, nullptr);
    }
    const VkSemaphore &handle() const { return _semaphore; }
    static Result<std::unique_ptr<Semaphore>> create(const Device &device);

private:
    const Device &_device;
    VkSemaphore _semaphore;
};

class Fence
{
public:
    Fence(const Device &device, VkFence fence) : _device(device), _fence(fence) {}
    void destroy()
    {
        vkDestroyFence(_device.handle(), _fence, nullptr);
    }

    const VkFence &handle() const { return _fence; }

    static Result<std::unique_ptr<Fence>> create(const Device &device);

    void wait_forever()
    {
        vkWaitForFences(_device.handle(), 1, &_fence, VK_TRUE, UINT64_MAX);
    }

    void reset()
    {
        vkResetFences(_device.handle(), 1, &_fence);
    }

private:
    const Device &_device;
    VkFence _fence;
};

} // namespace plt
