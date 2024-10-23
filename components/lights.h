#pragma once
#include "vk/pipeline.h"
#include "uniform_buffers.h"
namespace components 
{
    class PointLightsUniform : public vk::Uniform
    {
    public:
        PointLightsUniform();
        ~PointLightsUniform();
        void Set(uint32_t currentFrame, const 
            vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer) override;
        void SetLightActive(uint32_t id, bool flag) {
            assert(id < 16);
            mUniformBufferData.isActive[id] = flag? 1:0;
        }
        void SetLightPosition(uint32_t id, glm::vec3 pos) {
            assert(id < 16);
            mUniformBufferData.positions[id] = pos;
        }
        void SetLightColorAndIntensity(uint32_t id, glm::vec3 rgb, float intensity) {
            assert(id < 16);
            mUniformBufferData.colorAndIntensity[id] = glm::vec4(rgb, intensity);
        }
        
    private:
        PointLightsUniformBuffer mUniformBufferData{};

    };
}