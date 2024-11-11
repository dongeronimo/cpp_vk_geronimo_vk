#include "vma_helper.h"
#include <vk/device.h>
#include <vk/instance.h>
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
VkBuffer mem::VmaHelper::CreateBuffer(
    VkDeviceSize bufferSize,
    VkDeviceSize minAlignment,
    uint32_t amount
    )
{
    const VkDeviceSize alignedSize = (bufferSize + minAlignment - 1) & ~(minAlignment - 1);
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = 
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

mem::VmaHelper::~VmaHelper()
{
    vmaDestroyAllocator(allocator);
}