#include "render_pass.h"
#include "device.h"
#include <cassert>
#include "debug_utils.h"
namespace vk {
    RenderPass::RenderPass(const std::string& name, const std::array<float, 4>& markColor)
        :mName(name), mHash(utils::Hash(name)), mDebugMarkColor(markColor)
    {
        assert(name.size() > 0);
    }
    RenderPass::~RenderPass()
    {
        const auto dev = Device::gDevice->GetDevice();
        vkDestroyRenderPass(dev, mRenderPass, nullptr);
    }
    void RenderPass::BeginRenderPass(VkCommandBuffer cmdBuffer, uint32_t imageIndex, uint32_t frameNumber)
    {
        assert(mRenderPass != VK_NULL_HANDLE);//did you forget to set this in the ctor?
        assert(mClearValues.size() != 0);//the children have to define that value.
        assert(mExtent.width != 0 && mExtent.height != 0);//the children have to define this too.
        //begin the render pass of the render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = mRenderPass;
        renderPassInfo.framebuffer = GetFramebuffer(imageIndex);
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = mExtent;

        renderPassInfo.clearValueCount = static_cast<uint32_t>(mClearValues.size());
        renderPassInfo.pClearValues = mClearValues.data();
        vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vk::SetMark({ 0.0f, 0.8f, 4.0f, 1.0f }, mName, cmdBuffer);
        mDebugInUse = true;
    }
    void RenderPass::EndRenderPass(VkCommandBuffer cmdBuffer)
    {
        assert(mRenderPass != VK_NULL_HANDLE);
        assert(mDebugInUse);
        vkCmdEndRenderPass(cmdBuffer);
        vk::EndMark(cmdBuffer);
    }
}