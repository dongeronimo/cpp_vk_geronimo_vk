#include "volume_render_pass.h"

components::VolumeRenderPass::VolumeRenderPass(
    std::vector<VkFramebuffer> sharedFramebuffer)
    :vk::RenderPass("volumeRenderPass", {0.0f, 0.8f, 0.33f, 1.0f}),
    mSharedFramebuffer(sharedFramebuffer)
{
    mClearValues.clear();

}

VkFramebuffer components::VolumeRenderPass::GetFramebuffer(uint32_t imageIndex)
{
    return VkFramebuffer();
}
