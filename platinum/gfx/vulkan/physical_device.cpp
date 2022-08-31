#include <platinum/gfx/vulkan/vulkan.hpp>
#include <platinum/log/log.hpp>
#include <set>

#include <platinum/gfx/vulkan/physical_device.hpp>
namespace plt
{

bool PhysicalDevice::is_suitable(Surface &surface) const
{
    VkPhysicalDeviceProperties deviceProperties = this->properties();
    VkPhysicalDeviceFeatures deviceFeatures = this->features();

    if (!this->pick_queue_family_index(surface))
    {
        return false;
    }
    if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || !deviceFeatures.geometryShader)
    {
        return false;
    }

    if (!this->check_device_extension(PhysicalDevice::get_required_extension()))
    {
        return false;
    }

    SwapchainSupportDetails swapchainSupport = this->query_swapchain_support(surface);
    if (swapchainSupport.formats.size() == 0 || swapchainSupport.presentModes.size() == 0)
    {
        return false;
    }

    return false;
}

bool PhysicalDevice::check_device_extension(const VulkanExtensions &extensions) const
{
    uint32_t extensionCount = 0;

    vkEnumerateDeviceExtensionProperties(this->_physical_device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(this->_physical_device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
    for (const VkExtensionProperties &extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

int PhysicalDevice::suitability_score() const
{
    if (this->_physical_device == nullptr)
    {
        return -1;
    }
    int score = 0;
    VkPhysicalDeviceProperties deviceProperties = this->properties();

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

Result<VulkanQueueFamilyIndex> PhysicalDevice::pick_queue_family_index(Surface &surface) const
{
    VulkanQueueFamilyIndex index = {
        .graphicsFamily = -1,
        .presentFamily = -1,
    };

    auto families = this->queue_families();

    int i = 0;
    for (const auto &queue_family : families)
    {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            index.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;

        vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device, i, surface.handle(), &presentSupport);

        if (presentSupport)
        {
            index.presentFamily = i;
        }

        if (index.graphicsFamily >= 0 && index.presentFamily >= 0)
        {
            break;
        }

        i++;
    }

    if (index.graphicsFamily == -1 || index.presentFamily == -1)
    {
        return Result<VulkanQueueFamilyIndex>::err("Could not find a queue family supporting both graphics and present");
    }

    return index;
}

void PhysicalDevice::dump() const
{
    auto props = this->properties();
    info$(" == gpu {} dump == ", props.deviceName);

    info$(" - name: {}", props.deviceName);
    info$(" - score: {}", this->suitability_score());
    info$(" - type: {}", props.deviceType);
    info$(" - api-version: {}", props.apiVersion);
    info$(" - driver-version: {}", props.driverVersion);
    info$(" - max-compute-size: {}-{}-{}", props.limits.maxComputeWorkGroupSize[0], props.limits.maxComputeWorkGroupSize[1], props.limits.maxComputeWorkGroupSize[2]);
    info$(" - max-compute-count: {}-{}-{}", props.limits.maxComputeWorkGroupCount[0], props.limits.maxComputeWorkGroupCount[1], props.limits.maxComputeWorkGroupCount[2]);
    info$(" - max-image-dim-2D: {}", props.limits.maxImageDimension2D);
    info$(" - max-fb-dim: {}x{}", props.limits.maxFramebufferWidth, props.limits.maxFramebufferHeight);
    uint32_t extensionCount = 0;

    vkEnumerateDeviceExtensionProperties(this->_physical_device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(this->_physical_device, nullptr, &extensionCount, availableExtensions.data());

    info$(" - extensions: {}", availableExtensions.size());
    for (const VkExtensionProperties &extension : availableExtensions)
    {
        info$("   - {}", extension.extensionName);
    }
}

SwapchainSupportDetails PhysicalDevice::query_swapchain_support(Surface &surface) const
{
    SwapchainSupportDetails details = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, surface.handle(), &details.capabilities);

    uint32_t formatCount = 0;
    uint32_t presentModeCount = 0;

    vk_try(vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, surface.handle(), &formatCount, nullptr)).unwrap();
    vk_try(vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_device, surface.handle(), &presentModeCount, nullptr)).unwrap();

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vk_try(vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, surface.handle(), &formatCount, details.formats.data())).unwrap();
    }
    else
    {
        warn$("no format");
    }
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vk_try(vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_device, surface.handle(), &presentModeCount, details.presentModes.data())).unwrap();
    }
    else
    {
        warn$("no present");
    }

    return (details);
}

Result<std::unique_ptr<Device>> PhysicalDevice::create_logical_device(Surface &surface)
{

    VkDevice device;
    VulkanQueueFamilyIndex index = try$(pick_queue_family_index(surface));

    std::set<uint32_t> queues{
        uint32_t(index.graphicsFamily),
        uint32_t(index.presentFamily),
    };

    float queue_priority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for (auto &queue : queues)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queue;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queue_priority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    VulkanExtensions exts = this->get_required_extension();

    VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,

        .queueCreateInfoCount = uint32_t(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),

        .enabledExtensionCount = uint32_t(exts.size()),
        .ppEnabledExtensionNames = exts.data(),

        .pEnabledFeatures = &deviceFeatures,
    };

    vk_try$(vkCreateDevice(this->_physical_device, &createInfo, nullptr, &device));

    plt::Device dev = plt::Device::create(device, index);
    return Result<std::unique_ptr<Device>>(std::make_unique<Device>(std::move(dev)));
}
} // namespace plt
