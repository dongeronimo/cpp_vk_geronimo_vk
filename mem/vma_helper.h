#pragma once
#define WIN32_LEAN_AND_MEAN
//#define VMA_STATIC_VULKAN_FUNCTIONS 0
//#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
namespace mem {
    /// <summary>
    /// Helps our use of VMA. It hides VMA behind itself, so the rest of the program don't
    /// 
    /// </summary>
    class VmaHelper {
    public:
        /// <summary>
        /// The first time this function is called it'll create the allocator. To make
        /// sure we have the allocator as soon as possible i call it after i created
        /// the device.
        /// </summary>
        /// <returns></returns>
        static VmaHelper& GetInstance() {
            static VmaHelper instance;
            return instance;
        }
        /// <summary>
        /// Creates a uniform buffer for the type t, with it's allocation and
        /// allocation information. The buffer will be host coherent, host visible,
        /// random access and mapped. That means that you can get the VmaAllocationInfo::pMappedData 
        /// and memcpy to it.
        /// </summary>
        template <class t>
        void CreateUniformBufferFor(uint32_t amount, 
            VkBuffer& buffer, 
            VmaAllocation& allocation, 
            VmaAllocationInfo& allocationInfo) 
        {
            VkBufferCreateInfo bufferCreateInfo = {};
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size = sizeof(t) * amount;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
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
                &buffer, &allocation, &allocationInfo);
        }
        /// <summary>
        /// Creates an aligned uniform buffer. This function is intended to be used
        /// to create uniform buffers that will receive data via memcpy so the memory
        /// is VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, VMA_ALLOCATION_CREATE_MAPPED_BIT,
        /// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT and VK_MEMORY_PROPERTY_HOST_COHERENT_BIT.
        /// That means that you can get the VmaAllocationInfo::pMappedData and memcpy to it.
        /// </summary>
        /// <param name="bufferSize"></param>
        /// <param name="minAlignment"></param>
        /// <param name="amount"></param>
        /// <param name="buffer"></param>
        /// <param name="allocation"></param>
        /// <param name="allocInfo"></param>
        void CreateAlignedUniformBuffer(
            VkDeviceSize bufferSize,
            VkDeviceSize minAlignment,
            uint32_t amount,
            VkBuffer& buffer,
            VmaAllocation& allocation,
            VmaAllocationInfo& allocInfo
        );
        void FreeMemory(VmaAllocation& allocation);
        /// <summary>
        /// Remember to call this when shutting down the app. It should
        /// be one of the last calls, called while we still have a VkDevice.
        /// 
        /// If you fail to call this VMA will crash during shutdown.
        /// </summary>
        static void Destroy() {
            auto& instance = GetInstance();
            vmaDestroyAllocator(instance.allocator);
        }
    private:
        VmaAllocator allocator;
        VmaHelper();
        ~VmaHelper() = default;
    };
}

#define VMA_HELPER mem::VmaHelper::GetInstance()