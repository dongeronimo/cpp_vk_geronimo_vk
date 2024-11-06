#pragma once
#include <vulkan/vulkan.h>
#include "utils/hash.h"
#include <vector>
namespace components {
    class Renderable;
}
namespace vk
{
    class RenderPass;
    class Pipeline;
    VkDescriptorSetLayout ModelMatrixDescriptorSetLayout();
    VkShaderModule LoadShaderModule(VkDevice device, const std::string& name);
    class Uniform {
    public:
        virtual void SetUniform(
            uint32_t currentFrame, 
            const Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) = 0;
    };
    class Pipeline
    {
    public:
        Pipeline(const std::string name, const RenderPass& rp);
        virtual void Bind(VkCommandBuffer buffer, uint32_t currentFrame);
        void Unbind(VkCommandBuffer buffer);
        /// <summary>
        /// ALl objects that exist in the base class are destroyed by it.
        /// </summary>
        virtual ~Pipeline();
        void SetUniform(Uniform* uniform, 
            uint32_t currentFrame, 
            VkCommandBuffer cmdBuffer) {
            uniform->SetUniform(currentFrame, *this, cmdBuffer);
        }
        const hash_t mHash;
        const std::string mName;
        /// <summary>
        /// Called when we do resize. Some pipelines need to be recreated when we resize the surface
        /// </summary>
        virtual void DestroyPipeline();
        /// <summary>
        /// Draw the renderable using the pipeline
        /// </summary>
        /// <param name="r"></param>
        /// <param name="cmdBuffer"></param>
        virtual void Draw(components::Renderable& r, VkCommandBuffer cmdBuffer, uint32_t currentFrame);
        void AddRenderable(components::Renderable* r);
        void RemoveRenderable(components::Renderable* r);
        std::vector<components::Renderable*>& GetRenderables() {
            return mRenderables;
        }
    protected:
        std::vector<components::Renderable*> mRenderables;
        const RenderPass& mRenderPass;
        VkPipeline mPipeline = VK_NULL_HANDLE;
        VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
        std::vector< VkDescriptorSetLayout> mDescriptorSetLayouts;
        VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
        std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStage(VkShaderModule vert, VkShaderModule frag);
        std::vector<VkVertexInputAttributeDescription> DefaultAttributeDescription();
        VkVertexInputBindingDescription DefaultBindingDescription();
    };
}