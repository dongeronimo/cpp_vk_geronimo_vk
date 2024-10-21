#pragma once
#include "camera.h"
#include "vk/pipeline.h"
#include "solid_phong_pipeline.h"
#include "transform.h"
#include <vulkan/vulkan.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace components
{
    Camera::Camera(const std::string& n) :
        Transform(n), CameraUniform(), 
        mFOV(glm::radians(60.0f)), mRatio(1.0f), mZNear(1.0f), mZFar(100.0f)
    {

    }
    void Camera::Set(uint32_t currentFrame,const vk::Pipeline& pipeline,VkCommandBuffer cmdBuffer)
    {
        //update mCameraData
        glm::mat4 projection = glm::perspective(mFOV, mRatio, mZNear, mZFar);
        //GOTCHA: GLM is for opengl, the y coords are inverted. With this trick we the correct that
        //projection[1][1] *= -1;
        // View matrix
        glm::mat4 rotationMatrix = glm::toMat4(mOrientation);
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), - mPosition);
        glm::mat4 view = rotationMatrix * translationMatrix;
        mCameraData.proj = projection;
        mCameraData.view = view;
        mCameraData.viewPos = mPosition;
        CameraUniform::Set(currentFrame, pipeline, cmdBuffer);
    }
}