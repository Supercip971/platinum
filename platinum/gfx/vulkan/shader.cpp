#include <platinum/gfx/vulkan/shader.hpp>

namespace plt
{
Result<std::unique_ptr<ShaderProgram>> ShaderProgram::create_shader(const std::vector<char> &data, Device &device, ShaderProgramType type)
{
    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = data.size(),
        .pCode = (const uint32_t *)data.data(),
    };

    VkShaderModule _module;
    vkCreateShaderModule(device.handle(), &createInfo, nullptr, &_module);

    return plt::success(std::make_unique<ShaderProgram>(_module, device, type));
}

void ShaderProgram::destroy()
{
    vkDestroyShaderModule(this->_device.handle(), this->_module, nullptr);
}

Result<std::unique_ptr<ShaderPipeline>> ShaderPipeline::create(const ShaderPack &pack, Device &device)
{
    std::vector<VkPipelineShaderStageCreateInfo> stages;

    (void)device;

    if (pack.vertex)
    {
        VkPipelineShaderStageCreateInfo stage{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = pack.vertex->handle(),
            .pName = "main",
        };
        stages.push_back(stage);
    }

    if (pack.fragment)
    {
        VkPipelineShaderStageCreateInfo stage{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = pack.fragment->handle(),
            .pName = "main",
        };
        stages.push_back(stage);
    }

    return success(std::make_unique<ShaderPipeline>(std::move(stages)));
}
} // namespace plt
