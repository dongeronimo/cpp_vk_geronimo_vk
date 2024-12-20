#pragma once
#include <vulkan/vulkan.h>
#include "data_structures/ring_buffer.h"
namespace utils {
    uint32_t FindMemoryType(uint32_t typeFilter,
        VkMemoryPropertyFlags properties);

    VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

    static VkDeviceSize GetMinAlignment(VkPhysicalDevice physicalDevice) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        VkDeviceSize minAlignment = properties.limits.minUniformBufferOffsetAlignment;
        return minAlignment;
    }

    static VkDeviceSize AlignedSize(VkDeviceSize unalignedStructSize, uint32_t amount, VkPhysicalDevice physicalDevice) {
        auto minAlignment = GetMinAlignment(physicalDevice);
        VkDeviceSize alignedSize = (minAlignment + minAlignment - 1) & ~(minAlignment - 1);
        VkDeviceSize finalSize = alignedSize * amount;
        return finalSize;
    }

    

    void CreateAlignedBuffer(VkDeviceSize structNonAlignedSize, uint32_t amount, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer,
        VkDeviceMemory& bufferMemory);


    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer,
        VkDeviceMemory& bufferMemory);

    void TransitionImageLayout(VkImage image, 
        VkFormat format,
        VkImageLayout oldLayout, 
        VkImageLayout newLayout);

}