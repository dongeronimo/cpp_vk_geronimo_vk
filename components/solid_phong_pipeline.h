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
        virtual void SetUniform(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
        virtual ~CameraUniform() = default;
    };

    class ModelMatrixUniform : public vk::Uniform {
    public:
        ModelMatrixUniform(uint32_t mId):mModelId(mId){}
        const uint32_t mModelId;
        ModelUniformBuffer mModelData;
        virtual void SetUniform(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
        virtual ~ModelMatrixUniform() = default;
    };

    class SolidPhongPipeline : public vk::Pipeline {
    public:
        SolidPhongPipeline(const vk::RenderPass& rp);
        ~SolidPhongPipeline();
        void Bind(VkCommandBuffer buffer, uint32_t currentFrame) override;
        friend class CameraUniform;
        friend class ModelMatrixUniform;
        void Recreate();
    private:
        std::vector<VkVertexInputAttributeDescription> AttributeDescription();
        VkVertexInputBindingDescription BindingDescription();
        
        ring_buffer_t<VkBuffer> mCameraBuffer;
        ring_buffer_t<VkDeviceMemory> mCameraBufferMemory;
        ring_buffer_t<VkDescriptorSet> mCameraDescriptorSet;

        ring_buffer_t<VkBuffer> mModelBuffer;
        ring_buffer_t<VkDeviceMemory> mModelBufferMemory;
        ring_buffer_t<VkDescriptorSet> mModelDescriptorSet;
        //TODO shadow: add sampler to sample the shadow data
        //TODO shadow: update the shader to use the shadow map

            
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateCameraBuffer();
        void CreateDescriptorSet();
        void CreatePipelineLayout();
        void CreateModelBuffer();
        VkShaderModule mVertexShader, mFragmentShader;

    };
}