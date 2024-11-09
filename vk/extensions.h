#pragma once
#include <vector>
#include <string>
#include <vulkan/vulkan.h>
namespace vk {
    /// <summary>
    /// List all extensions available
    /// </summary>
    /// <returns></returns>
    std::vector<VkExtensionProperties> GetExtensions();
    /// <summary>
    /// List of required extensions
    /// </summary>
    std::vector<const char*> getRequiredExtensions(bool enableValidationLayers);

    static bool ExtensionIsPresent(
        const std::vector<VkExtensionProperties>& extensions,
        const char* extensionName) {
        for (const auto& extension : extensions) {
            const std::string currentExtensionName(extension.extensionName);
            const std::string desiredExtensionName(extensionName);
            if (currentExtensionName == desiredExtensionName)
                return true;
        }
        return false;
    }
}