#pragma once
#include "vk/render_pass.h"
#include "data_structures/ring_buffer.h"
#include <memory>
#include <vector>
namespace vk {
    class SwapChain;
    class DepthBuffer;
}

namespace components {
    class VolumeRenderPass : public vk::RenderPass {
    public:
        VolumeRenderPass(std::vector<VkFramebuffer> sharedFramebuffer);
        // Inherited via RenderPass
        VkFramebuffer GetFramebuffer(uint32_t imageIndex) override;
    private:
        std::vector<VkFramebuffer> mSharedFramebuffer;


    };
}