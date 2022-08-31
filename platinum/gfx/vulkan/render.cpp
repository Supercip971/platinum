#include <platinum/gfx/render.hpp>
#include <platinum/gfx/vulkan/instance.hpp>
#include <platinum/gfx/vulkan/shader.hpp>
#include <platinum/gfx/vulkan/vulkan.hpp>

namespace plt
{
Result<> VkGfx::initialize_ender_frames()
{
    std::vector<std::unique_ptr<CommandBuffer>> command_buffers =
        try$(CommandBuffer::create_buffers(*_device, *_swapchain, *_command_pool, max_frames_in_flight));

    std::vector<std::unique_ptr<Semaphore>> image_available_semaphores(max_frames_in_flight);
    std::vector<std::unique_ptr<Semaphore>> render_finished_semaphores(max_frames_in_flight);
    std::vector<std::unique_ptr<Fence>> in_flight_fences(max_frames_in_flight);

    for (int i = 0; i < max_frames_in_flight; i++)
    {
        _working_frames[i].command_buffer = std::move(command_buffers[i]);
        _working_frames[i].image_available_semaphore = try$(Semaphore::create(*_device));
        _working_frames[i].render_finished_semaphore = try$(Semaphore::create(*_device));
        _working_frames[i].in_flight_fence = try$(Fence::create(*_device));
    }
    return Result<>::ok({});
}

Result<> VkGfx::cleanup_swapchain()
{
    _framebuffers->destroy();
    _swapchain->destroy_image_view();
    _swapchain->destroy();
    return Result<>::ok({});
}

Result<> VkGfx::recreate_swapchain()
{
    _device->wait_idle();

    cleanup_swapchain();

    _swapchain = try$(Swapchain::create(_pdevice->query_swapchain_support(*_surface), *_device, *_surface));
    try$(_swapchain->create_image_view());
    _framebuffers = try$(Framebuffers::create(*_device, *_swapchain, *_render_pass));

    return Result<>::ok({});
}
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

    this->initialize_ender_frames();
    //_command_buffer = try$(CommandBuffer::create(*_device, *_swapchain, *_command_pool));

    // _image_available_semaphore = try$(Semaphore::create(*_device));
    // _render_finished_semaphore = try$(Semaphore::create(*_device));

    // _in_flight_fence = try$(Fence::create(*_device));
    _current_frame = 0;

    target_surface.add_callback(WindowCallbackType::WINDOW_CALLBACK_RESIZE, [&](Window &window, WindowCallback callback)
                                {

                                    while(window.width().unwrap() == 0 || window.height().unwrap() == 0)
                                    {
                                        window.update();
                                    }

                                    info$("Resizing window to: {}x{}", (window.width().unwrap()),  (window.height().unwrap()));
        this->resized_framebuffer = true; });
    return Result<>::ok({});
};
Result<> VkGfx::draw_frame()
{

    auto &working_frame = _working_frames.at(_current_frame);
    working_frame.in_flight_fence->wait_forever();
    auto image_d = try$(_swapchain->acquire_next_image(*working_frame.image_available_semaphore));

    if (image_d.out_of_date_swapchain && resized_framebuffer)
    {
        this->recreate_swapchain();
        return draw_frame();
    }

    working_frame.in_flight_fence->reset();

    uint32_t image = image_d.id;

    working_frame.command_buffer->reset();

    working_frame.command_buffer->begin_command();
    {
        working_frame.command_buffer->begin_render_pass(*_swapchain, *_render_pass, *_framebuffers, image);

        working_frame.command_buffer->bind_pipeline(*_gfx_pipeline);

        working_frame.command_buffer->draw(*_swapchain, 3, 1);

        working_frame.command_buffer->end_render_pass();
    }
    working_frame.command_buffer->end_command();

    working_frame.command_buffer->submit(*working_frame.image_available_semaphore, *working_frame.render_finished_semaphore, *working_frame.in_flight_fence);

    auto r2 = _swapchain->present(image, *working_frame.render_finished_semaphore).unwrap();

    if (r2.out_of_date_swapchain || resized_framebuffer)
    {
        resized_framebuffer = false;
        this->recreate_swapchain();
    }

    _current_frame = (_current_frame + 1) % max_frames_in_flight;

    return Result<>::ok({});
}
void VkGfx::deinit()
{
    _device->wait_idle();

    cleanup_swapchain();
    for (int i = 0; i < max_frames_in_flight; i++)
    {

        _working_frames[i].in_flight_fence->destroy();

        _working_frames[i].image_available_semaphore->destroy();
        _working_frames[i].render_finished_semaphore->destroy();

        _working_frames[i].command_buffer->destroy();
    }

    _command_pool->destroy();
    _gfx_pipeline->destroy();
    _gfx_pipeline_layout->destroy();
    _render_pass->destroy();
    _default_fragment->destroy();
    _default_vertex->destroy();

    _device->destroy();
    _instance->destroy_surface(*_surface);
    _debugger.detach(*_instance);
    Instance::destroy(*_instance);
}

} // namespace plt
