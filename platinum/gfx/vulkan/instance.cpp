#include <platinum/gfx/vulkan/debug.hpp>
#include <platinum/gfx/vulkan/instance.hpp>
#include <platinum/gfx/vulkan/surface.hpp>
#include <platinum/gfx/vulkan/vulkan.hpp>
#include <platinum/window/window.hpp>

namespace plt
{
Result<int> Instance::check_extensions(const plt::ReqExtensions &required_exts)
{
    uint32_t ext_count = 0;

    vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr);

    std::vector<VkExtensionProperties> extensions(ext_count);

    vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, extensions.data());

    for (const char *ext : required_exts)
    {
        bool found = false;
        for (const VkExtensionProperties &extension : extensions)
        {
            if (strcmp(ext, extension.extensionName) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return Result<int>::err("extension not found: {}", ext);
        }
    }

    return Result<int>::ok(0);
}

Result<int> Instance::check_layers(const plt::ReqLayers &required_layer)
{
    uint32_t layer_count = 0;

    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> layers(layer_count);

    vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

    for (const char *req_layer : required_layer)
    {
        bool found = false;
        for (const VkLayerProperties &layer : layers)
        {
            if (strcmp(req_layer, layer.layerName) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return Result<int>::err("layer not found: {}", req_layer);
        }
    }

    return Result<int>::ok(0);
}

Result<Instance::InstancePtr> Instance::create(const plt::ReqExtensions &required_ext, const plt::ReqLayers &required_layers)
{

    try$(Instance::check_extensions(required_ext));
    try$(Instance::check_layers(required_layers));

    Instance instance;
    instance.instance = VK_NULL_HANDLE;

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Platinum",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Platinum",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,

        .enabledLayerCount = uint32_t(required_layers.size()),
        .ppEnabledLayerNames = required_layers.data(),

        .enabledExtensionCount = uint32_t(required_ext.size()),
        .ppEnabledExtensionNames = required_ext.data(),
    };

    VulkanDebug::attach_instance_construction(create_info);

    vk_try$(vkCreateInstance(&create_info, nullptr, &instance.instance));

    return Result<Instance::InstancePtr>::ok(std::make_unique<Instance>(instance));
}

void Instance::destroy(Instance &instance)
{
    vkDestroyInstance(instance.instance, nullptr);
}

Result<std::unique_ptr<plt::PhysicalDevice>> Instance::pick_physical_device()
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);

    if (count == 0)
    {
        return Result<std::unique_ptr<plt::PhysicalDevice>>::err("no physical device found");
    }

    std::vector<VkPhysicalDevice> physical_devices(count);
    vkEnumeratePhysicalDevices(instance, &count, physical_devices.data());

    // TODO(): we are using a lot of vulkan call here, with vulkan_get_properties,
    // this may be slow and we may want to cache the score instead.

    plt::PhysicalDevice physical_device;
    for (const auto &device : physical_devices)
    {
        auto dev = plt::PhysicalDevice(device);
        info$("founded device: {}", dev.properties().deviceName);

        dev.dump();

        if (dev.suitability_score() > physical_device.suitability_score())
        {
            physical_device = dev;
        }
    }

    if (physical_device.suitability_score() <= 0)
    {
        return Result<std::unique_ptr<plt::PhysicalDevice>>::err("no suitable physical device found");
    }
    else
    {
        return Result<std::unique_ptr<plt::PhysicalDevice>>::ok(std::make_unique<plt::PhysicalDevice>(physical_device));
    }
}

void Instance::destroy_surface(plt::Surface &surface)
{
    vkDestroySurfaceKHR(instance, surface._surface, nullptr);
}

Result<std::unique_ptr<plt::Surface>> Instance::create_surface(plt::Window &target_surface)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    vk_try$(glfwCreateWindowSurface(instance, (GLFWwindow *)target_surface.handle(), nullptr, &surface));
    VkExtent2D extent = {
        .width = uint32_t(try$(target_surface.width())),
        .height = uint32_t(try$(target_surface.height())),
    };

    return Result<std::unique_ptr<plt::Surface>>::ok(std::make_unique<plt::Surface>(surface, extent));
}
} // namespace plt
