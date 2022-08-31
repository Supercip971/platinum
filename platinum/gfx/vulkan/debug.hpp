#pragma once
#include <platinum/gfx/vulkan/instance.hpp>
#include <vulkan/vulkan.h>
namespace plt
{
struct VulkanDebug
{

    VkDebugUtilsMessengerEXT debugMessenger;
    static VkDebugUtilsMessengerCreateInfoEXT debug_creation_information();
    static Result<> attach_instance_construction(VkInstanceCreateInfo &createInfo);
    Result<> attach(Instance &instance);
    void detach(Instance &instance);
};
}; // namespace plt
