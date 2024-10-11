#pragma once
#include "transform.h"
#include "mesh.h"
#include "solid_phong_pipeline.h"
namespace components
{
    class Renderable : public Transform, public ModelMatrixUniform {
    public:
        Renderable(const std::string& n, Mesh& mesh);
        ~Renderable();
        virtual void Set(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
        const Mesh& mMesh;
    };

}