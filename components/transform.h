#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
namespace components
{
    const glm::vec3 V_UP = { 0,1,0 };
    static uint32_t gTransformCounter = 0;
    class Transform
    {
    public:
        Transform(const std::string& n):
            mName(n), mId(gTransformCounter),
            mPosition({0,0,0}), mOrientation(glm::quat())
        {
            gTransformCounter++;
            //LookTo(glm::vec3(0, 0, 1));//default orientation is +z=forward
        }

        bool areNearlyParallelOrAntiparallel(const glm::vec3& v1, const glm::vec3& v2, float epsilon = 0.0001f) {
            float dotProduct = glm::dot(glm::normalize(v1), glm::normalize(v2));
            return glm::abs(dotProduct - 1.0f) < epsilon || glm::abs(dotProduct + 1.0f) < epsilon;
        }

        /// <summary>
        /// Rotates so that it's looking from mOrientation to target
        /// </summary>
        /// <param name="target"></param>
        void LookTo(glm::vec3 target) {
            //gambiarra pra lidar com o NaN quando a direção e o vup são paralelos/antiparalelos
            if (areNearlyParallelOrAntiparallel(V_UP, (target - mPosition))) {
                target = target + glm::vec3{0, 0, 0.00001f};
                glm::mat4 lookAtMat = glm::lookAt(mPosition, target, V_UP);
                mOrientation = glm::toQuat(lookAtMat);
            }
            else {
                glm::mat4 lookAtMat = glm::lookAt(mPosition, target, V_UP);
                mOrientation = glm::toQuat(lookAtMat);
            }
        }
        glm::vec3 mPosition;
        glm::quat mOrientation;
        const std::string& mName;
        const uint32_t mId;
    };
}