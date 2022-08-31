#include <platinum/gfx/vulkan/vulkan.hpp>

#include <platinum/gfx/vulkan/logical_device.hpp>

namespace plt
{
void Device::destroy()
{
    vkDestroyDevice(this->_device, nullptr);
}
void Device::wait_idle()
{
    vkDeviceWaitIdle(this->_device);
}
} // namespace plt
