#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include "device.h"
#include "instance.h"
#include <string>
namespace vk {
    VkSampler MakeLinearRepeat2DSampler(const std::string& name);

    void CreateImage(uint32_t width, uint32_t height, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties, VkImage& image,
        VkDeviceMemory& imageMemory);

    VkImageView CreateImageView(VkImage image, VkFormat format, 
        VkImageAspectFlags aspectFlags);
}