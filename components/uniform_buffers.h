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