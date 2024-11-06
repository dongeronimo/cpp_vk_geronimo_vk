#pragma once
#include <glm/glm.hpp>
namespace components
{
    struct alignas(16) CameraUniformBuffer {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
        alignas(16) glm::vec3 viewPos;
    };

    struct alignas(16) ModelUniformBuffer {
        alignas(16) glm::mat4 model;
    };

    struct alignas(16) LightSpaceMatrixUniformBuffer {
        alignas(16) glm::mat4 lightMatrix;
    };

    struct alignas(16) DirectionalLightPropertiesUniformBuffer {
        alignas(16) glm::vec3 direction;
        alignas(16) glm::mat4 lightSpaceMatrix;
        alignas(16) glm::vec4 colorAndIntensity;
    };

    struct alignas(128) PhongProperties {
        alignas(128) glm::vec3 ambientColor;
        alignas(128) float ambientStrength;
        alignas(128) float specularStrength;
    };
}