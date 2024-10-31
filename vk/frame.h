#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "data_structures/ring_buffer.h"
#include <functional>

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
        uint32_t mImageIndex;
        const size_t mCurrentFrame;
        VkCommandBuffer CommandBuffer();
        void EndFrame();
        std::function<void()> OnResize;
        float DeltaTime();
    private:
        const ring_buffer_t<VkCommandBuffer>& commandBuffers;
        vk::SyncronizationService& mSyncService;
        vk::SwapChain& mSwapChain;
        bool IsDegenerateFramebuffer() const;

    };
}