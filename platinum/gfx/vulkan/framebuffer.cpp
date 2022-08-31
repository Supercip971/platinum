#include <platinum/gfx/vulkan/framebuffer.hpp>
#include <platinum/gfx/vulkan/vulkan.hpp>
namespace plt
{
Result<std::unique_ptr<Framebuffers>> Framebuffers::create(Device &device, Swapchain &swapchain, RenderPass &render_pass)
{
    VkFramebuffers framebuffers;
    framebuffers.resize(swapchain.image_view().size());
    for (uint32_t i = 0; i < swapchain.image_view().size(); ++i)
    {
        VkImageView attachments[] = {
            swapchain.image_view()[i],
        };
        VkFramebufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = render_pass.handle();
        info.attachmentCount = 1;
        info.pAttachments = attachments;
        info.width = swapchain.extent().width;
        info.height = swapchain.extent().height;
        info.layers = 1;
        vk_try$(vkCreateFramebuffer(device.handle(), &info, nullptr, &framebuffers[i]));
    }
    return success(std::make_unique<Framebuffers>(framebuffers, device, swapchain, render_pass));
}

void Framebuffers::destroy()
{
    for (auto &framebuffer : _framebuffers)
    {
        vkDestroyFramebuffer(_device.handle(), framebuffer, nullptr);
    }
}
} // namespace plt
