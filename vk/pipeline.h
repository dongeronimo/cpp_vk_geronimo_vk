#pragma once
#include <vulkan/vulkan.h>
#include "utils/hash.h"
namespace components {
    class Renderable;
}
namespace vk
{
    class RenderPass;
    class Pipeline;
    VkShaderModule LoadShaderModule(VkDevice device, const std::string& name);
    class Uniform {
    public:
        virtual void Set(uint32_t currentFrame, 
            const Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) = 0;
    };
    class Pipeline
    {
    public:
        Pipeline(const std::string name, const RenderPass& rp);
        virtual void Bind(VkCommandBuffer buffer, uint32_t currentFrame);
        /// <summary>
        /// ALl objects that exist in the base class are destroyed by it.
        /// </summary>
        virtual ~Pipeline();
        void SetUniform(Uniform* uniform, uint32_t currentFrame, VkCommandBuffer cmdBuffer) {
            uniform->Set(currentFrame, *this, cmdBuffer);
        }
        const hash_t mHash;
        const std::string mName;
        virtual void Draw(components::Renderable& r, VkCommandBuffer cmdBuffer);
    protected:
        const RenderPass& mRenderPass;
        VkPipeline mPipeline = VK_NULL_HANDLE;
        VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    };
}