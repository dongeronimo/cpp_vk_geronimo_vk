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
    class DirectionalLightUniform : public vk::Uniform {
    public:
        DirectionalLightPropertiesUniformBuffer mLightData;
        virtual void SetUniform(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
        virtual ~DirectionalLightUniform() = default;
    };
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
        SolidPhongPipeline(const vk::RenderPass& rp, const std::vector<VkImageView>& shadowMapImageViews,
            VkImageView texture, VkSampler sampler);
        ~SolidPhongPipeline();
        void ActivateShadowMap(uint32_t framebufferImageNumber, VkCommandBuffer buffer);
        void Bind(VkCommandBuffer buffer, uint32_t currentFrame) override;
        friend class CameraUniform;
        friend class ModelMatrixUniform;
        friend class DirectionalLightUniform;
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

        std::vector<VkDescriptorSet> mShadowMapDescriptorSet;
        VkDescriptorSet mPhongTexturesDescriptorSet;
        
        ring_buffer_t<VkBuffer> mDirectionalLightBuffer;
        ring_buffer_t<VkDeviceMemory> mDirectionalLightMemory;
        ring_buffer_t<VkDescriptorSet> mDirectionalLightDescriptorSet;

        VkSampler mShadowDepthSampler = VK_NULL_HANDLE;

        void CreateDepthSampler();
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateCameraBuffer();
        void CreateDirectionalLightDataBuffer();
        void CreateDescriptorSet(const std::vector<VkImageView>& shadowMapImageViews,
            VkSampler phongSampler, VkImageView phongDiffuseImageView);
        void CreatePipelineLayout();
        void CreateModelBuffer();
        VkShaderModule mVertexShader, mFragmentShader;

    };
}