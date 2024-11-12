#pragma once
#include "vk\pipeline.h"
#include "data_structures/ring_buffer.h"
#include "uniform_buffers.h"
#include "mem/vma_helper.h"
namespace components
{
    class ModelMatrixUniform;
    class RenderPass;
    class DirectionalLightShadowMapPipeline : public vk::Pipeline {
    public:
        DirectionalLightShadowMapPipeline(const vk::RenderPass& rp);
        ~DirectionalLightShadowMapPipeline();
        void Recreate();
        void Bind(VkCommandBuffer buffer, uint32_t currentFrame) override;
        friend class ModelMatrixUniform;
        void SetLightMatrix(VkCommandBuffer buffer, 
            LightSpaceMatrixUniformBuffer& lm);
    private:
        VkShaderModule mVertexShader, mFragmentShader;
        //ring_buffer_t<VkBuffer> mModelBuffer;
        //ring_buffer_t<VkDeviceMemory> mModelBufferMemory;
        ring_buffer_t<VkDescriptorSet> mModelDescriptorSet;

        ring_buffer_t<VkBuffer> mModelBuffer;
        ring_buffer_t<VmaAllocation> mModelAllocation;
        ring_buffer_t<VmaAllocationInfo> mModelAllocationInfo;
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateModelBuffer();
        void CreatePipelineLayout();
        void CreateDescriptorSet();
    };
}