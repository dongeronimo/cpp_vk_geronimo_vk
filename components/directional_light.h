#pragma once
#include "transform.h"
#include "solid_phong_pipeline.h"
namespace components {
    class DirectionalLight :
        public Transform, 
        public DirectionalLightUniform {
    public:
        glm::mat4 GetLightMatrix();
        DirectionalLight();
        void SetUniform(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
    };
}