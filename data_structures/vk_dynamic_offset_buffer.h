//#pragma once
//#include <vulkan/vulkan.h>
//#include <vector>
//#include <data_structures/ring_buffer.h>
//template <typename T, uint32_t SZ>
//class VkDynamicOffsetBuffer {
//public:
//    VkDynamicOffsetBuffer(
//        VkBufferUsageFlags usage,
//        VkMemoryPropertyFlags memoryProperties,
//        VkDevice device, 
//        VkPhysicalDevice physicalDevice
//    ) : 
//        mDevice(device), mPhysicalDevice(physicalDevice)
//    {
//        //determine alignment
//        VkPhysicalDeviceProperties deviceProperties;
//        vkGetPhysicalDeviceProperties(mPhysicalDevice, &deviceProperties);
//        size_t minUboAlignment = deviceProperties.limits.minUniformBufferOffsetAlignment;
//        mDynamicAlignment = sizeof(T);
//        if(minUboAlignment > 0)
//            mDynamicAlignment = (mDynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
//        //create the buffers
//        const size_t alignedBufferSize = SZ * mDynamicAlignment;
//        const size_t allBuffersAlignedSize = alignedBufferSize * mBuffer.size();
//        VkBufferCreateInfo bigBufferInfo = {};
//        bigBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//        bigBufferInfo.size = allBuffersAlignedSize;
//        bigBufferInfo.usage = usage;
//        bigBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//        VkBuffer bigBuffer;
//        vkCreateBuffer(device, &bigBufferInfo, nullptr, &bigBuffer);
//        VkMemoryRequirements memRequirements;
//        vkGetBufferMemoryRequirements(device, bigBuffer, &memRequirements);
//        VkMemoryAllocateInfo allocInfo = {};
//        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//        allocInfo.allocationSize = memRequirements.size;
//        allocInfo.memoryTypeIndex = utils::FindMemoryType(memRequirements.memoryTypeBits, memoryProperties);
//        vkAllocateMemory(device, &allocInfo, nullptr, &mMemory);
//        vkDestroyBuffer(device, bigBuffer, nullptr);
//        for (auto i = 0; i < mBuffer.size(); i++) {
//            VkBufferCreateInfo bufferInfo = {};
//            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//            bufferInfo.size = alignedBufferSize;
//            bufferInfo.usage = usage;
//            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//            vkCreateBuffer(device, &mBuffer[i], nullptr, &bufferInfo);
//            vkBindBufferMemory(device, mBuffer[i], mMemory, i * alignedBufferSize);
//            mBases[i] = i * alignedBufferSize;
//        }
//        for (auto f = 0; f < MAX_FRAMES_IN_FLIGHT; f++) {
//            for (auto i = 0; i < N; i++) {
//                mOffsets[f][i] = f * alignedBufferSize + i * mDynamicAlignment;
//            }
//        }
//    }
//    ~VkDynamicOffsetBuffer() {
//        vkFreeMemory(mDevice, mMemory, nullptr);
//        for (auto& b : mBuffer) {
//            vkDestroyBuffer(device, b, nullptr);
//        }
//    }
//
//    void memcpy(const T& data, uint32_t frame, uint32_t i)
//    {
//        // Calculate the base and offset
//        auto base = mBases[frame];
//        auto offset = mOffsets[frame][i];
//        auto basePlusOffset = base + offset;
//
//        // Map the memory
//        void* mappedMemory;
//        vkMapMemory(mDevice, mMemory, basePlusOffset, sizeof(T), 0, &mappedMemory);
//
//        // Copy the data
//        memcpy(mappedMemory, &data, sizeof(T));
//
//        // Unmap the memory
//        vkUnmapMemory(mDevice, mMemory);
//    }
//    const uint32_t Items() { return SZ; }
//    const size_t DynamicAlignment()const { return mDynamicAlignment; }
//    ring_buffer_t<VkBuffer>& GetBuffer() {
//        return mBuffer;
//    }
//private:
//    size_t mDynamicAlignment;
//    const VkDevice mDevice;
//    const VkPhysicalDevice mPhysicalDevice;
//    ring_buffer_t<VkBuffer> mBuffer;
//    VkDeviceMemory mMemory;
//    ring_buffer_t<std::vector<VkDeviceSize>> mOffsets;
//    ring_buffer_t<VkDeviceSize> mBases;
//};