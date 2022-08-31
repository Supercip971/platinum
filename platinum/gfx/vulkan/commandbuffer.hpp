#pragma once
#pragma once
#include <platinum/gfx/vulkan/framebuffer.hpp>
#include <platinum/gfx/vulkan/pass.hpp>
#include <platinum/gfx/vulkan/pipeline.hpp>
#include <platinum/gfx/vulkan/shader.hpp>
#include <platinum/gfx/vulkan/swapchain.hpp>
#include <vulkan/vulkan.h>

#include <platinum/gfx/vulkan/logical_device.hpp>

namespace plt
{

class CommandPool : public NoCopy
{
public:
    CommandPool(VkCommandPool pool, Device &device)
        : _command_pool(pool), _device(device)
    {
    }
    static Result<std::unique_ptr<CommandPool>> create(Device &device, int family_index);
    void destroy();
    const VkCommandPool &handle() const { return _command_pool; }

private:
    VkCommandPool _command_pool;

    Device &_device;
};

class CommandBuffer : public NoCopy
{
public:
    CommandBuffer(VkCommandBuffer &fb, Device &device, Swapchain &swapchain, CommandPool &pool)
        : _command_buffer(fb), _device(device), _swapchain(swapchain), _pool(pool)
    {
    }
    static Result<std::unique_ptr<CommandBuffer>> create(Device &device, Swapchain &swapchain, CommandPool &pool);
    static Result<std::vector<std::unique_ptr<CommandBuffer>>> create_buffers(Device &device, Swapchain &swapchain, CommandPool &pool, int count);

    void destroy();

    const VkCommandBuffer &handle() const { return _command_buffer; }

    Result<> begin_command();

    CommandBuffer &begin_render_pass(RenderPass &pass, Framebuffers &fb, int frame_id);

    CommandBuffer &bind_pipeline(GraphicPipeline &pipeline)
    {
        vkCmdBindPipeline(_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle());
        return *this;
    }

    CommandBuffer &draw(uint32_t vertex_count, uint32_t instance_count = 1);
    CommandBuffer &end_render_pass()
    {
        vkCmdEndRenderPass(_command_buffer);
        return *this;
    }

    void reset()
    {
        vkResetCommandBuffer(_command_buffer, 0);
    }

    Result<> end_command();

    Result<> submit(Semaphore &wait_semaphore, Semaphore &signal_semaphore, Fence &signaled_fence);

private:
    VkCommandBuffer _command_buffer;

    Device &_device;
    Swapchain &_swapchain;
    CommandPool &_pool;
};

} // namespace plt
