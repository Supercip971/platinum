#include <platinum/gfx/vulkan/commandbuffer.hpp>
#include <platinum/gfx/vulkan/vulkan.hpp>
namespace plt
{

Result<std::unique_ptr<CommandPool>> CommandPool::create(Device &device, int family_index)
{
    VkCommandPool pool;
    VkCommandPoolCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,

        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = uint32_t(family_index),
    };

    vk_try$(vkCreateCommandPool(device.handle(), &info, nullptr, &pool));
    return success(std::make_unique<CommandPool>(pool, device));
}

void CommandPool::destroy()
{
    vkDestroyCommandPool(_device.handle(), _command_pool, nullptr);
}

Result<std::unique_ptr<CommandBuffer>> CommandBuffer::create(Device &device, Swapchain &swapchain, CommandPool &pool)
{
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool.handle(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VkCommandBuffer command_buffer;

    vk_try$(vkAllocateCommandBuffers(device.handle(), &allocInfo, &command_buffer));

    return success(std::make_unique<CommandBuffer>(command_buffer, device, swapchain, pool));
}

void CommandBuffer::destroy()
{
    vkFreeCommandBuffers(_device.handle(), _pool.handle(), 1, &_command_buffer);
}

Result<> CommandBuffer::begin_command()
{

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        //   .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        .pInheritanceInfo = nullptr,
    };
    vk_try$(vkBeginCommandBuffer(_command_buffer, &beginInfo));

    return Result<>::ok({});
}

CommandBuffer &CommandBuffer::begin_render_pass(RenderPass &pass, Framebuffers &fb, int frame_id)
{
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = pass.handle(),
        .framebuffer = fb.handles()[frame_id],
        .renderArea = {
            .offset = {0, 0},
            .extent = _swapchain.extent(),
        },
        .clearValueCount = 1,
        .pClearValues = &clearColor,
    };

    vkCmdBeginRenderPass(_command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    return *this;
}

CommandBuffer &CommandBuffer::draw(uint32_t vertex_count, uint32_t instance_count)
{
    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = float(_swapchain.extent().width),
        .height = float(_swapchain.extent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(_command_buffer, 0, 1, &viewport);

    VkRect2D scissor{
        .offset = {0, 0},
        .extent = _swapchain.extent(),
    };

    vkCmdSetScissor(_command_buffer, 0, 1, &scissor);

    vkCmdDraw(_command_buffer, vertex_count, instance_count, 0, 0);

    return *this;
}
Result<> CommandBuffer::end_command()
{
    vk_try$(vkEndCommandBuffer(_command_buffer));
    return Result<>::ok({});
}

Result<> CommandBuffer::submit(Semaphore &wait_semaphore, Semaphore &signal_semaphore, Fence &signaled_fence)
{
    std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    std::array<VkSemaphore, 1> waitSemaphores = {wait_semaphore.handle()};
    std::array<VkSemaphore, 1> signalSemaphores = {signal_semaphore.handle()};

    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = waitSemaphores.size(),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = 1,
        .pCommandBuffers = &_command_buffer,
        .signalSemaphoreCount = signalSemaphores.size(),
        .pSignalSemaphores = signalSemaphores.data(),
    };

    vk_try$(vkQueueSubmit(_device.graphics_queue(), 1, &submitInfo, signaled_fence.handle()));
    return Result<>::ok({});
}
} // namespace plt
