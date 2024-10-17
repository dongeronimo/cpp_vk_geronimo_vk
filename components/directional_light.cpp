#include "directional_light.h"
//TODO light: Directional light can't inherit from transform.
namespace components
{
    //static glm::vec3 ld = glm::normalize(glm::vec3(0.0f,-1.0f, 0.0f));
    static glm::vec3 ld = glm::normalize(-glm::vec3(10.0f, 5.0f, 0.0f));

    glm::mat4 DirectionalLight::GetLightMatrix()
    {
        glm::vec3 lightPos = -ld * 5.0f;//TODO light: do not use hardcoded distance
        glm::vec3 lightTarget = { 0,0,0 };//TODO light: calculate the center of the visible objects, based on the frustum
        glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, { 0,1,0 });
        glm::mat4 lightProj = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.001f, 10.f);//TODO light: calculate based on the objects visible on the frustum
        lightProj[1][1] *= -1;
        glm::mat4 lightMatrix = lightProj * lightView;
        return lightMatrix;
    }
    void DirectionalLight::SetColor(glm::vec3 rgb)
    {
        mLightData.colorAndIntensity.r = rgb.r;
        mLightData.colorAndIntensity.g = rgb.g;
        mLightData.colorAndIntensity.b = rgb.b;

    }
    void DirectionalLight::SetIntensity(float a)
    {
        mLightData.colorAndIntensity.a = a;
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
        mLightData.direction = ld;
        mLightData.lightSpaceMatrix = GetLightMatrix();

        DirectionalLightUniform::SetUniform(currentFrame, pipeline, cmdBuffer);
    }
}