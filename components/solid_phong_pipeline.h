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
    class PhongMaterialUniform;
    class PointLightsUniform;
    class CameraUniform : public vk::Uniform {
    public:
        CameraUniformBuffer mCameraData;
        virtual void Set(uint32_t currentFrame, 
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
        virtual ~CameraUniform() = default;
    };

    class ModelMatrixUniform : public vk::Uniform {
    public:
        ModelMatrixUniform(uint32_t mId):mModelId(mId){}
        const uint32_t mModelId;
        ModelUniformBuffer mModelData;
        virtual void Set(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
        virtual ~ModelMatrixUniform() = default;
    };

    class SolidPhongPipeline : public vk::Pipeline {
    public:
        SolidPhongPipeline(const std::string& name, const vk::RenderPass& rp, VkSampler phongTextureSampler, VkImageView textureImageView);
        ~SolidPhongPipeline();
        void Bind(VkCommandBuffer buffer, uint32_t currentFrame) override;
        void Draw(components::Renderable& r, VkCommandBuffer cmdBuffer, uint32_t currentFrame) override;
        friend class CameraUniform;
        friend class ModelMatrixUniform;
        friend class PointLightsUniform;
        friend class PhongMaterialUniform;
        void Recreate();
    private:
        //TODO memory: merge all device memories into a single big one to save allocations
        std::vector<VkVertexInputAttributeDescription> AttributeDescription();
        VkVertexInputBindingDescription BindingDescription();
        //camera things
        ring_buffer_t<VkBuffer> mCameraBuffer;
        ring_buffer_t<VkDeviceMemory> mCameraBufferMemory;
        ring_buffer_t<VkDescriptorSet> mCameraAndPointLightDescriptorSet;
        //model things
        ring_buffer_t<VkBuffer> mModelBuffer;
        ring_buffer_t<VkDeviceMemory> mModelBufferMemory;
        ring_buffer_t<VkDescriptorSet> mModelDescriptorSet;
        //phong texture sampler things
        VkSampler mPhongTextureSampler;
        VkDescriptorSet mPhongTextureSamplerDescriptorSet;
        //point lights - don't have their own descriptor set because they are in the same set that camera is
        ring_buffer_t<VkBuffer> mPointLightsBuffer;
        ring_buffer_t<VkDeviceMemory> mPointLightsMemory;
        //Phong material
        ring_buffer_t<VkBuffer> mPhongBuffer;
        ring_buffer_t<VkDeviceMemory> mPhongMemory;
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateCameraBuffer();
        void CreateDescriptorSet(VkImageView textureImageView);
        void CreatePipelineLayout();
        void CreateModelBuffer();
        void CreatePointLightsBuffer();

        VkShaderModule mVertexShader, mFragmentShader;
        //VkRingBuffer<CameraUniformBuffer> mCameraBuffer;

    };
    
    class PhongMaterialUniform : public vk::Uniform {
    public:
        PhongMaterialUniform(uint32_t modelId);
        virtual void Set(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer);
        void SetDiffuseColor(glm::vec3 color, float intensity) {
            mMaterial.diffuseColorAndIntensity = glm::vec4(color, intensity);
        }
        void SetSpecularColor(glm::vec3 color, float intensity) {
            mMaterial.specularColorAndIntensity = glm::vec4(color, intensity);
        }
    private:
        const uint32_t mModelId;
        components::PhongMaterialUniformBuffer mMaterial;
    };
}