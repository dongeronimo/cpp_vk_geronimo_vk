#pragma once
#include "transform.h"
#include "mesh.h"
#include "solid_phong_pipeline.h"
namespace components
{
    class Renderable;

    class PhongModelMatrixUniform: public ModelMatrixUniform {
    public:
        PhongModelMatrixUniform(Renderable& owner,uint32_t mModelId);
        virtual void SetUniform(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
        Renderable& mOwner;
    };

    class Renderable : public Transform {
    public:
        Renderable(const std::string& n, Mesh& mesh);
        ~Renderable();
        const Mesh& mMesh;
        void SetUniforms(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer);
        friend class PhongModelMatrixUniform;
        uint32_t GetModelId() { return mModelMatrixUniform.mModelId; }
    private:
        PhongModelMatrixUniform mModelMatrixUniform;
    };

}