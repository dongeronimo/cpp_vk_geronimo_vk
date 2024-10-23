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
    /// <summary>
    /// Up to 16 point lights
    /// </summary>
    struct alignas(16) PointLightsUniformBuffer {
        alignas(16) glm::vec3 positions[16];
        alignas(16) glm::vec4 colorAndIntensity[16];
        alignas(16) uint32_t isActive[16];
    };

    struct alignas(16) MaterialUniformBuffer {
        alignas(16) glm::vec3 ambient;
        alignas(16) glm::vec3 diffuse;
        alignas(16) glm::vec3 specular;
        alignas(16) float shininess;
    };

    struct alignas(16) LightPositionUniformBuffer {
        alignas(16) glm::vec3 lightPos;
    };
}