#pragma once
#include "transform.h"
#include "mesh.h"
#include "solid_phong_pipeline.h"
#include <queue>
#include <memory>
namespace components
{
    class Renderable;
    class Animation;

    class PhongModelMatrixUniform: public ModelMatrixUniform {
    public:
        PhongModelMatrixUniform(Renderable& owner,uint32_t mModelId);
        virtual void SetUniform(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
        Renderable& mOwner;
    };
    class RenderablePhongProperties : public PhongPropertiesUniform {
    public:
        RenderablePhongProperties(Renderable& owner, uint32_t mModelId);
        virtual void SetUniform(uint32_t currentFrame,
            const vk::Pipeline& pipeline,
            VkCommandBuffer cmdBuffer) override;
        float mAmbientStrength;
        float mSpecularStrength;
        glm::vec3 mAmbientColor;
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
        std::unique_ptr<RenderablePhongProperties> mPhongProperties = nullptr;
        void AdvanceAnimation(float deltaTime);
        void EnqueueAnimation(std::shared_ptr<Animation> a) {
            assert(a != nullptr);
            mAnimations.push(a);
        }
    private:
        PhongModelMatrixUniform mModelMatrixUniform;
        std::queue<std::shared_ptr<Animation>> mAnimations;
    };

}