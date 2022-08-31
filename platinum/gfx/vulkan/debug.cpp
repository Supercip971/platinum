
#include <platinum/gfx/vulkan/debug.hpp>
#include <platinum/gfx/vulkan/vulkan.hpp>
#include <platinum/log/log.hpp>

namespace plt
{
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    (void)pUserData;

    std::string type = "";

    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
    {
        type += "general ";
    }
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
    {
        type += "validat ";
    }
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
    {
        type += "perf    ";
    }

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        error$("vulkan[{}]: {}",  type, pCallbackData->pMessage);
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        warn$("vulkan[{}]: {}", type,  pCallbackData->pMessage);
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        info$("vulkan[{}]: {}", type,  pCallbackData->pMessage);
    }
    else
    {
        debug$("vulkan[{}]: {}", type,  pCallbackData->pMessage);
    }

    return VK_FALSE;
}

static VkResult IVkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (fn != nullptr)
    {
        return fn(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void IVkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (fn != nullptr)
    {
        fn(instance, debugMessenger, pAllocator);
    }
}

Result<> VulkanDebug::attach(Instance &instance)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = VulkanDebug::debug_creation_information();
    vk_try$(IVkCreateDebugUtilsMessengerEXT(instance.instance, &createInfo, nullptr, &debugMessenger));

    return Result<>::ok({});
}

void VulkanDebug::detach(Instance &instance)
{
    IVkDestroyDebugUtilsMessengerEXT(instance.instance, debugMessenger, nullptr);
}

VkDebugUtilsMessengerCreateInfoEXT VulkanDebug::debug_creation_information()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
    };
    return createInfo;
}

Result<> VulkanDebug::attach_instance_construction(VkInstanceCreateInfo &createInfo)
{
    static const auto debug_struct_create = VulkanDebug::debug_creation_information();
    createInfo.pNext = &debug_struct_create;
    return Result<>::ok({});
}
}; // namespace plt
