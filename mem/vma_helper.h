#pragma once
#define WIN32_LEAN_AND_MEAN
//#define VMA_STATIC_VULKAN_FUNCTIONS 0
//#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
namespace mem {
    class VmaHelper {
    public:
        static VmaHelper& GetInstance() {
            static VmaHelper instance;
            return instance;
        }
        VkBuffer CreateBuffer(
            VkDeviceSize bufferSize,
            VkDeviceSize minAlignment,
            uint32_t amount
        );
    private:
        VmaAllocator allocator;
        VmaHelper();
        ~VmaHelper();
    };
}

#define VMA_HELPER mem::VmaHelper::GetInstance()