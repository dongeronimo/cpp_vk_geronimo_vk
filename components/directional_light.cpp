#include "directional_light.h"

namespace components
{
    glm::mat4 DirectionalLight::GetLightMatrix()
    {
        glm::vec3 lightDirection(mOrientation.x, mOrientation.y, mOrientation.z);
        glm::vec3 lightPos = -lightDirection * 30.0f;//TODO light: do not use hardcoded distance
        glm::vec3 lightTarget = { 0,0,0 };//TODO light: calculate the center of the visible objects, based on the frustum
        glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, { 0,1,0 });
        glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.f);//TODO light: calculate based on the objects visible on the frustum
        glm::mat4 lightMatrix = lightProj * lightView;
        return lightMatrix;
    }
    DirectionalLight::DirectionalLight()
        :Transform("DirectionalLight"),
        DirectionalLightUniform()
    {
    }
    /*    struct alignas(16) DirectionalLightPropertiesUniformBuffer {
        alignas(16) glm::vec3 direction;
        alignas(16) glm::mat4 lightSpaceMatrix;
        alignas(16) glm::vec4 colorAndIntensity;
    };*/
    void DirectionalLight::SetUniform(uint32_t currentFrame,
        const vk::Pipeline& pipeline,
        VkCommandBuffer cmdBuffer)
    {
        glm::vec3 lightDirection(mOrientation.x, mOrientation.y, mOrientation.z);
        //glm::vec3 lightPos = -lightDirection * 30.0f;//TODO light: do not use hardcoded distance
        //glm::vec3 lightTarget = { 0,0,0 };//TODO light: calculate the center of the visible objects, based on the frustum
        //glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, { 0,1,0 });
        //glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.f);//TODO light: calculate based on the objects visible on the frustum
        //glm::mat4 lightMatrix = lightProj * lightView;

        mLightData.direction = lightDirection;
        mLightData.colorAndIntensity = { 1.0f, 1.0f, 1.0f, 1.0f };
        mLightData.lightSpaceMatrix = GetLightMatrix();

        DirectionalLightUniform::SetUniform(currentFrame, pipeline, cmdBuffer);
    }
}