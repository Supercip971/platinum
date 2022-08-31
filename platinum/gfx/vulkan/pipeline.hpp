#pragma once
#include <platinum/gfx/vulkan/pass.hpp>
#include <platinum/gfx/vulkan/shader.hpp>
#include <platinum/gfx/vulkan/swapchain.hpp>
#include <vulkan/vulkan.h>

#include <platinum/gfx/vulkan/logical_device.hpp>

namespace plt
{

class GraphicPipelineLayout : public NoCopy
{
public:
    GraphicPipelineLayout(const Device &device, VkPipelineLayout layout) : _device(device), _layout(layout) {}
    static Result<std::unique_ptr<GraphicPipelineLayout>> create(Device &dev);

    const VkPipelineLayout &handle() const
    {
        return _layout;
    }

    void destroy()
    {
        vkDestroyPipelineLayout(_device.handle(), _layout, nullptr);
    }

private:
    const Device &_device;
    VkPipelineLayout _layout;
};

class GraphicPipeline : public NoCopy
{
public:
    GraphicPipeline(Device &device, GraphicPipelineLayout &pipeline_layout, VkPipeline pipeline)
        : _layout(pipeline_layout),
          _pipeline(pipeline),
          _device(device)
    {
    }

    const VkPipeline &handle() const
    {
        return _pipeline;
    }
    static Result<std::unique_ptr<GraphicPipeline>>
    create(RenderPass &pass, ShaderPipeline &pack, GraphicPipelineLayout &layout, Device &device, const Swapchain &swapchain);

    void destroy();

private:
    GraphicPipelineLayout &_layout;
    VkPipeline _pipeline;
    const Device &_device;
};
} // namespace plt
