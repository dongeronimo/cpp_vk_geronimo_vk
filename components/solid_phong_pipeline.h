#pragma once
#include "vk\pipeline.h"
#include "uniform_buffers.h"
#include "data_structures/ring_buffer.h"
#include <vector>
//#include "data_structures/vk_ring_buffer.h"
//#include "data_structures/vk_dynamic_offset_buffer.h"
const uint32_t MAX_NUMBER_OF_OBJS = 1000;
namespace vk {
    class RenderPass;
}
namespace components
{
    class CameraUniform : public vk::Uniform {
    public:
        CameraUniformBuffer mCameraData;
        void Set(uint32_t currentFrame, 
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) const override;
    };

    class SolidPhongPipeline : public vk::Pipeline {
    public:
        SolidPhongPipeline(const vk::RenderPass& rp);
        ~SolidPhongPipeline();
        void Bind(VkCommandBuffer buffer, uint32_t currentFrame) override;
        friend class CameraUniform;
    private:
        std::vector<VkVertexInputAttributeDescription> AttributeDescription();
        VkVertexInputBindingDescription BindingDescription();
        
        ring_buffer_t<VkBuffer> mCameraBuffer;
        ring_buffer_t<VkDeviceMemory> mCameraBufferMemory;
        ring_buffer_t<VkDescriptorSet> mCameraDescriptorSet;

        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateCameraBuffer();
        void CreateDescriptorSet();
        void CreatePipelineLayout();
        VkShaderModule mVertexShader, mFragmentShader;
        //VkRingBuffer<CameraUniformBuffer> mCameraBuffer;
        /////////////////////OLD        
        //ring_buffer_t<VkDescriptorSet> mDescriptorSets;
        //VkRingBuffer<CameraUniformBuffer> mCameraBuffer;
        //VkRingBuffer<LightSpaceMatrixUniformBuffer> mLightSpaceMatrixBuffer;
        //VkRingBuffer<LightPositionUniformBuffer> mLightPositionBuffer;
        //VkDynamicOffsetBuffer<ModelUniformBuffer, MAX_NUMBER_OF_OBJS> mModelUniformBuffer;
        //VkDynamicOffsetBuffer<MaterialUniformBuffer, MAX_NUMBER_OF_OBJS> mMaterialUniformBuffer;
    };
}