#pragma once

#include <platinum/gfx/render.hpp>
#include <platinum/gfx/vulkan/commandbuffer.hpp>
#include <platinum/gfx/vulkan/debug.hpp>
#include <platinum/gfx/vulkan/instance.hpp>
#include <platinum/gfx/vulkan/shader.hpp>
#include <platinum/gfx/vulkan/swapchain.hpp>
#include <platinum/gfx/vulkan/sync.hpp>

#include <platinum/gfx/vulkan/physical_device.hpp>
namespace plt
{

[[maybe_unused]] constexpr static inline Result<> vk_try(VkResult result)
{
    if (result == VK_SUCCESS)
    {
        return Result<>::ok({});
    }
    uint64_t r = result;
    return Result<>::err("vulkan error: {}", (int32_t)r);
}

#define vk_try$(expr) try$(vk_try(expr))

struct WorkingFrame
{
    std::unique_ptr<CommandBuffer> command_buffer;
    std::unique_ptr<Semaphore> image_available_semaphore;
    std::unique_ptr<Semaphore> render_finished_semaphore;
    std::unique_ptr<Fence> in_flight_fence;
};
class VkGfx : public gfx
{
public:
    virtual Result<> init(Window &target_surface) final;

    virtual Result<> draw_frame() final;

    virtual void deinit() final;

private:
    constexpr static bool enable_validation_layer = true;
    constexpr static int max_frames_in_flight = 2;

    Result<> initialize_ender_frames();
    plt::VulkanDebug _debugger;
    std::unique_ptr<plt::Instance> _instance;
    std::unique_ptr<plt::PhysicalDevice> _pdevice;
    std::unique_ptr<plt::Device> _device;
    std::unique_ptr<plt::Surface> _surface;
    std::unique_ptr<plt::Swapchain> _swapchain;
    std::unique_ptr<plt::ShaderProgram> _default_vertex;
    std::unique_ptr<plt::ShaderProgram> _default_fragment;
    std::unique_ptr<plt::ShaderPipeline> _shader_pipeline;
    std::unique_ptr<plt::RenderPass> _render_pass;

    std::unique_ptr<plt::GraphicPipelineLayout> _gfx_pipeline_layout;
    std::unique_ptr<plt::GraphicPipeline> _gfx_pipeline;
    std::unique_ptr<plt::Framebuffers> _framebuffers;
    std::unique_ptr<plt::CommandPool> _command_pool;
    std::unique_ptr<plt::CommandBuffer> _command_buffer;

    std::unique_ptr<plt::Fence> _in_flight_fence;
    std::unique_ptr<plt::Semaphore> _image_available_semaphore;
    std::unique_ptr<plt::Semaphore> _render_finished_semaphore;
};

} // namespace plt
