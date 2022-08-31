#pragma once
#include <platinum/io/file.hpp>
#include <vulkan/vulkan.h>

#include <platinum/gfx/vulkan/logical_device.hpp>

namespace plt
{
enum class ShaderProgramType
{
    Vertex,
    Fragment,
    Geometry,
    TessEval,
    TessControl,
    Compute,
    Count
};
class ShaderProgram : public NoCopy
{
public:
    static Result<std::unique_ptr<ShaderProgram>> create_shader(const std::vector<char> &data, Device &device, ShaderProgramType type);

    static Result<std::unique_ptr<ShaderProgram>> create_shader_from_path(const std::string &path, Device &device, ShaderProgramType type)
    {
        auto d = try$(File::read_buf(path));
        return ShaderProgram::create_shader(d, device, type);
    }

    VkShaderModule handle() const { return _module; };

    ShaderProgramType type() const { return _type; };

    void destroy();

    ShaderProgram(ShaderProgram &&other) = default;

    ShaderProgram(VkShaderModule module, const Device &device, ShaderProgramType type)
        : _module(module), _device(device), _type(type) {}

private:
    VkShaderModule _module;
    const Device &_device;
    ShaderProgramType _type;
};

struct ShaderPack
{
    const ShaderProgram *vertex = nullptr;
    const ShaderProgram *fragment = nullptr;
    const ShaderProgram *compute = nullptr;
};

class ShaderPipeline : public NoCopy
{
public:
    static Result<std::unique_ptr<ShaderPipeline>> create(const ShaderPack &pack, Device &device);
    std::vector<VkPipelineShaderStageCreateInfo> &creation_info() { return _stage_creation_info; };

    ShaderPipeline(std::vector<VkPipelineShaderStageCreateInfo> stage_creation_info)
        : _stage_creation_info(std::move(stage_creation_info)) {}

private:
    std::vector<VkPipelineShaderStageCreateInfo> _stage_creation_info;
};
} // namespace plt
