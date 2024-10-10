#pragma once
#include "solid_phong_pipeline.h"
#include "transform.h"
#include <vulkan/vulkan.h>
namespace vk {
    class Pipeline;
}
namespace components
{
    class Camera : public Transform, public CameraUniform{
    public:
        Camera(const std::string& n);
        float mFOV; //in rad
        float mRatio;
        float mZNear;
        float mZFar;
        void Set(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;


    };
}