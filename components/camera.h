#pragma once
#include "solid_phong_pipeline.h"
#include "transform.h"
#include <vulkan/vulkan.h>
namespace vk {
    class Pipeline;
}
namespace components {
    class CameraTest : public CameraUniform {
    public:
        void Set(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer) override {
            glm::vec3  viewPos{ 20, 10,20 };
            glm::mat4 projMat = glm::perspective(glm::radians(60.0f),
                (float)SCREEN_WIDTH/(float)SCREEN_HEIGH, 0.1f, 100.0f);
            projMat[1][1] *= -1;
            glm::mat4 viewMat = glm::lookAt(viewPos, { 0.0f,0.0f,0.0f }, V_UP);
            mCameraData.proj = projMat;
            mCameraData.view = viewMat;
            mCameraData.viewPos = viewPos;
            CameraUniform::Set(currentFrame, pipeline, cmdBuffer);
        }
    };
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