#pragma once
#include <platinum/gfx/vulkan/swapchain.hpp>
#include <platinum/utility/trait.hpp>

#include <platinum/gfx/vulkan/logical_device.hpp>
namespace plt
{
class RenderPass : public NoCopy
{
public:
    static Result<std::unique_ptr<RenderPass>> create(Device &device, const Swapchain &swapchain);

    RenderPass(const Device &device, VkRenderPass pass) : _device(device), _render_pass(pass) {}
    void destroy()
    {
        vkDestroyRenderPass(_device.handle(), _render_pass, nullptr);
    }

    const VkRenderPass &handle() const { return _render_pass; }

private:
    const Device &_device;
    VkRenderPass _render_pass;
};
} // namespace plt
