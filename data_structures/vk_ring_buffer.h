//#pragma once
//#include "ring_buffer.h"
//#include <vulkan/vulkan.h>
//#include <utils/vk_utils.h>
//
//template <typename T>
//class VkRingBuffer {
//public:
//    VkRingBuffer(
//        VkBufferUsageFlags usage, 
//        VkMemoryPropertyFlags memoryProperties,
//        VkDevice device
//    ):mDevice(device) {
//        uint32_t numBuffers = MAX_FRAMES_IN_FLIGHT;
//        VkDeviceMemory deviceMemory;
//        VkDeviceSize offsets[MAX_FRAMES_IN_FLIGHT];   // To store the offsets of each buffer
//         mTotalSize = 0;         // Total size of the memory block
//        VkDeviceSize currentOffset = 0;     // Tracks the current offset for each buffer
//        VkDeviceSize bufferSize = sizeof(T);
//        VkDeviceSize alignment = 0;
//        uint32_t memoryTypeBits = 0;
//        // Step 1: Create buffers and calculate total memory requirements
//        for (int i = 0; i < numBuffers; i++) {
//            // Create buffer
//            VkBufferCreateInfo bufferCreateInfo = {};
//            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//            bufferCreateInfo.size = bufferSize;  // Same size for all buffers
//            bufferCreateInfo.usage = usage;
//            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//            vkCreateBuffer(device, &bufferCreateInfo, nullptr, &mBuffers[i]);
//            // Get memory requirements for the first buffer (since all have the same size, just check once)
//            if (i == 0) {
//                VkMemoryRequirements memReq;
//                vkGetBufferMemoryRequirements(device, buffers[i], &memReq);
//                alignment = memReq.alignment;  // Same alignment for all buffers
//                memoryTypeBits = memReq.memoryTypeBits;
//            }
//            // Align the current offset
//            currentOffset = (currentOffset + alignment - 1) & ~(alignment - 1);  // Align the offset
//            offsets[i] = currentOffset;  // Store the aligned offset for this buffer
//            currentOffset += bufferSize;  // Update offset for next buffer
//        }
//        // The total size of the VkDeviceMemory block
//        mTotalSize = currentOffset;
//        // Step 2: Allocate memory block for all buffers
//        VkMemoryAllocateInfo allocInfo = {};
//        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//        allocInfo.allocationSize = mTotalSize;
//        uint32_t memoryTypeIndex = utils::FindMemoryType(mem, memoryProperties);
//        allocInfo.memoryTypeIndex = memoryTypeIndex;
//
//        vkAllocateMemory(device, &allocInfo, nullptr, &deviceMemory);
//
//        // Step 3: Bind each buffer to the shared memory block at its respective offset
//        for (int i = 0; i < numBuffers; i++) {
//            vkBindBufferMemory(device, buffers[i], deviceMemory, offsets[i]);
//            mOffsets[i] = offsets[i];
//        }   
//
//        void* mappedMemory;
//        vkMapMemory(mDevice, mMemory, 0, mTotalSize, 0, &mappedMemory);
//        uintptr_t base = reinterpret_cast<uintptr_t>(mappedMemory);
//        vkUnmapMemory(mDevice, mMemory);
//    }
//
//    void memcpy(const T& data, uint32_t i) {
//        uintptr_t basePlusOffset = mBase + mOffsets[i];
//        void* dest = reinterpret_cast<void*>(basePlusOffset);
//        memcpy(dest, data, sizeof(T));
//    }
//    ~VkRingBuffer() {
//        for (auto& b : mBuffers)
//            vkDestroyBuffer(mDevice, b, nullptr);
//        vkFreeMemory(mDevice, mMemory, nullptr);
//    }
//    ring_buffer_t<VkBuffer>& GetBuffer() {
//        return mBuffers;
//    }
//private:
//    uintptr_t mBase;
//    VkDeviceSize mTotalSize;
//    VkDevice mDevice;
//    ring_buffer_t<VkBuffer> mBuffers;
//    VkDeviceMemory mMemory;
//    ring_buffer_t<VkDeviceSize> mOffsets;
//};