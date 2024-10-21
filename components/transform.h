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
            mPosition({0,0,0}), 
            mOrientation(
                glm::quat()
            )
        {
            gTransformCounter++; 
        }
        /// <summary>
        /// Rotates so that it's looking from mOrientation to target
        /// </summary>
        /// <param name="target"></param>
        void LookTo(glm::vec3 target) {
            target.y = -target.y;
            glm::mat4 lookAtMat = glm::lookAt(mPosition, target, V_UP);
            mOrientation = glm::toQuat(lookAtMat);
        }
        void SetPosition(glm::vec3 v) {
           //v.y = -v.y;
           /*            float _x = v.x;
            v.x = v.z;
            v.z = _x;*/
            mPosition = v;
        }
        glm::quat mOrientation;
        const std::string& mName;
        const uint32_t mId;
    protected:
        glm::vec3 mPosition;
    };
}