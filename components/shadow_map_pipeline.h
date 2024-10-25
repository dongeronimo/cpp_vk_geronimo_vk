#pragma once
#include "vk\pipeline.h"
#include "data_structures/ring_buffer.h"
#include "uniform_buffers.h"
namespace components
{
    class ModelMatrixUniform;
    class RenderPass;
    class ShadowMapPipeline : public vk::Pipeline,
        public vk::IModelBufferMemoryAcessor 
    {
    public:
        ShadowMapPipeline(const std::string& name, const vk::RenderPass& rp);
        ~ShadowMapPipeline();
        void Recreate();
        void Bind(VkCommandBuffer buffer, uint32_t currentFrame) override;
        friend class ModelMatrixUniform;
        void SetLightMatrix(VkCommandBuffer buffer,
            LightSpaceMatrixUniformBuffer& lm);
        void Draw(components::Renderable& r, VkCommandBuffer cmdBuffer, uint32_t currentFrame);
        // Inherited via IModelBufferMemoryAcessor
        const VkDeviceMemory GetModelBufferMemory(uint32_t currentFrame) const override;
    private:
        VkShaderModule mVertexShader, mFragmentShader;
        ring_buffer_t<VkBuffer> mModelBuffer;
        ring_buffer_t<VkDeviceMemory> mModelBufferMemory;
        ring_buffer_t<VkDescriptorSet> mModelDescriptorSet;
        std::vector<VkVertexInputAttributeDescription> AttributeDescription();
        VkVertexInputBindingDescription BindingDescription();
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateModelBuffer();
        void CreatePipelineLayout();
        void CreateDescriptorSet();

        
    };
}