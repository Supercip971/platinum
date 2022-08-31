#include <platinum/gfx/vulkan/sync.hpp>
#include <platinum/gfx/vulkan/vulkan.hpp>

#include <platinum/gfx/vulkan/logical_device.hpp>

namespace plt
{
Result<std::unique_ptr<Semaphore>> Semaphore::create(const Device &device)
{
    VkSemaphoreCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkSemaphore semaphore;
    vk_try$(vkCreateSemaphore(device.handle(), &info, nullptr, &semaphore));
    return success(std::make_unique<Semaphore>(device, semaphore));
};

Result<std::unique_ptr<Fence>> Fence::create(const Device &device)
{
    VkFenceCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    VkFence fence;

    vk_try$(vkCreateFence(device.handle(), &info, nullptr, &fence));

    return success(std::make_unique<Fence>(device, fence));
}
} // namespace plt
