#include "vma_helper.h"
#include <vk/device.h>
#include <vk/instance.h>
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#include <utils/vk_utils.h>

void mem::VmaHelper::CreateAlignedBuffer(
    VkDeviceSize bufferSize,
    VkDeviceSize minAlignment,
    uint32_t amount,
    VkBuffer& buffer,
    VkBufferUsageFlags bufferUsage,
    VmaAllocation& allocation,
    VmaAllocationInfo& allocInfo
    )
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = bufferSize * amount;
    bufferCreateInfo.usage = bufferUsage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //memory properties
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | 
        VMA_ALLOCATION_CREATE_MAPPED_BIT;
    //create and map the memory
    vmaCreateBuffer(allocator, &bufferCreateInfo, &allocCreateInfo, 
        &buffer, &allocation, &allocInfo);
    
}
void mem::VmaHelper::CreateBufferInGPU(VkDeviceSize bufferSize, uint32_t amount, VkBuffer& buffer, VkBufferUsageFlags bufferUsage, VmaAllocation& allocation, VmaAllocationInfo& allocInfo)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = bufferSize * amount;
    bufferCreateInfo.usage = bufferUsage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //memory properties
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    //create and map the memory
    vmaCreateBuffer(allocator, &bufferCreateInfo, &allocCreateInfo,
        &buffer, &allocation, &allocInfo);

}
void mem::VmaHelper::FreeMemory(VmaAllocation& allocation)
{
    vmaFreeMemory(allocator, allocation);
}
mem::VmaHelper::VmaHelper()
{
    const VkDevice device = vk::Device::gDevice->GetDevice();
    const VkPhysicalDevice physicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
    const VkInstance instance = vk::Instance::gInstance->GetInstance();
    const uint32_t apiVersion = VK_API_VERSION_1_3;
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    allocatorCreateInfo.vulkanApiVersion = apiVersion;
    allocatorCreateInfo.physicalDevice = physicalDevice;
    allocatorCreateInfo.device = device;
    allocatorCreateInfo.instance = instance;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
    vmaCreateAllocator(&allocatorCreateInfo, &allocator);

}



