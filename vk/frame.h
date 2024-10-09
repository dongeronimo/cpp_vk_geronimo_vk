#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "data_structures/ring_buffer.h"
namespace vk {
    class SyncronizationService;
    class SwapChain;
}
namespace vk
{
    class Frame
    {
    public:
        Frame(
            const ring_buffer_t<VkCommandBuffer>& commandBuffers,
            size_t currentFrame,
            vk::SyncronizationService& syncService,
            vk::SwapChain& swapChain);
        void Foobar();
        bool BeginFrame();
        uint32_t ImageIndex() { return mImageIndex; }
        const size_t mCurrentFrame;
        VkCommandBuffer CommandBuffer();
        void EndFrame();
    private:
        const ring_buffer_t<VkCommandBuffer>& commandBuffers;
        vk::SyncronizationService& mSyncService;
        vk::SwapChain& mSwapChain;
        bool IsDegenerateFramebuffer() const;
        uint32_t mImageIndex;
    };
}