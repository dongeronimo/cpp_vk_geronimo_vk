#include "directional_light.h"
//TODO light: Directional light can't inherit from transform.
namespace components
{
    static glm::vec3 o = { 40,0,0 };
    static glm::vec3 t = { 0,0,0 };

    glm::mat4 DirectionalLight::GetLightMatrix()
    {
        glm::mat4 lightView = glm::lookAt(o, t, V_UP);
        glm::mat4 lightProj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.f);//TODO light: calculate based on the objects visible on the frustum
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
    DirectionalLight::DirectionalLight():
        DirectionalLightUniform()
    {
    }

    void DirectionalLight::SetUniform(uint32_t currentFrame,
        const vk::Pipeline& pipeline,
        VkCommandBuffer cmdBuffer)
    {
        mLightData.direction = glm::normalize(t - o);
        mLightData.lightSpaceMatrix = GetLightMatrix();
        DirectionalLightUniform::SetUniform(currentFrame, pipeline, cmdBuffer);
    }
}