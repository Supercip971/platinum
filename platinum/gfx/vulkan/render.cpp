#include <platinum/gfx/render.hpp>
#include <platinum/gfx/vulkan/instance.hpp>
#include <platinum/gfx/vulkan/shader.hpp>
#include <platinum/gfx/vulkan/vulkan.hpp>

namespace plt
{
Result<> VkGfx::init(Window &target_surface)
{

    ReqExtensions required_extensions = {
        Window::required_extensions(),
    };

    ReqLayers required_layers = {};

    if (enable_validation_layer)
    {
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        required_layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    _instance = try$(Instance::create(required_extensions, required_layers));
    _surface = try$(_instance->create_surface(target_surface));
    _debugger.attach(*_instance);
    _pdevice = try$(_instance->pick_physical_device());

    _device = try$(_pdevice->create_logical_device(*_surface));
    _swapchain = try$(Swapchain::create(_pdevice->query_swapchain_support(*_surface), *_device, *_surface));
    try$(_swapchain->create_image_view());

    _default_fragment = try$(ShaderProgram::create_shader_from_path("out/gen/platinum/shaders/shade-fs.spv", *_device, ShaderProgramType::Fragment));
    _default_vertex = try$(ShaderProgram::create_shader_from_path("out/gen/platinum/shaders/shade-vs.spv", *_device, ShaderProgramType::Vertex));

    _shader_pipeline = try$(
        ShaderPipeline::create(
            ShaderPack{
                .vertex = (_default_vertex.get()),
                .fragment = (_default_fragment.get()),
            },
            *_device));

    _render_pass = try$(RenderPass::create(*_device, *_swapchain));
    _gfx_pipeline_layout = try$(GraphicPipelineLayout::create(*_device));
    // I really don't like the fact that we are passing that much args in the constructor.
    _gfx_pipeline = try$(GraphicPipeline::create(*_render_pass, *_shader_pipeline, *_gfx_pipeline_layout, *_device, *_swapchain));

    _framebuffers = try$(Framebuffers::create(*_device, *_swapchain, *_render_pass));
    _command_pool = try$(CommandPool::create(*_device, _device->index().graphicsFamily));

    _command_buffer = try$(CommandBuffer::create(*_device, *_swapchain, *_command_pool));

    _image_available_semaphore = try$(Semaphore::create(*_device));
    _render_finished_semaphore = try$(Semaphore::create(*_device));

    _in_flight_fence = try$(Fence::create(*_device));

    return Result<>::ok({});
};
Result<> VkGfx::draw_frame()
{
    _in_flight_fence->wait_forever();
    _in_flight_fence->reset();

    uint32_t image = try$(_swapchain->acquire_next_image(*_image_available_semaphore));

    _command_buffer->reset();

    _command_buffer->begin_command();
    {
        _command_buffer->begin_render_pass(*_render_pass, *_framebuffers, image);

        _command_buffer->bind_pipeline(*_gfx_pipeline);

        _command_buffer->draw(3, 1);

        _command_buffer->end_render_pass();
    }
    _command_buffer->end_command();

    _command_buffer->submit(*_image_available_semaphore, *_render_finished_semaphore, *_in_flight_fence);
    _swapchain->present(image, *_render_finished_semaphore);

    return Result<>::ok({});
}
void VkGfx::deinit()
{
    _device->wait_idle();
    _in_flight_fence->destroy();
    _image_available_semaphore->destroy();
    _render_finished_semaphore->destroy();

    _command_buffer->destroy();
    _command_pool->destroy();
    _framebuffers->destroy();
    _gfx_pipeline->destroy();
    _gfx_pipeline_layout->destroy();
    _render_pass->destroy();
    _default_fragment->destroy();
    _default_vertex->destroy();

    _swapchain->destroy_image_view();
    _swapchain->destroy();
    _device->destroy();
    _instance->destroy_surface(*_surface);
    _debugger.detach(*_instance);
    Instance::destroy(*_instance);
}

} // namespace plt
