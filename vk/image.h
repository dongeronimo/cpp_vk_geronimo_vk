#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include "device.h"
#include "instance.h"
namespace vk {
    void CreateImage(uint32_t width, uint32_t height, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties, VkImage& image,
        VkDeviceMemory& imageMemory);

    VkImageView CreateImageView(VkImage image, VkFormat format, 
        VkImageAspectFlags aspectFlags);
}