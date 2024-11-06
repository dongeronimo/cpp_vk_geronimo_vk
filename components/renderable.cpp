#include "renderable.h"
#include <array>
#include <stdexcept>
#include "animation.h"
enum MODEL_ID_STATE {model_id_free, model_id_used};

std::vector<MODEL_ID_STATE> gAvailableModelIDs(MAX_NUMBER_OF_OBJS, model_id_free);

uint32_t GetAvailableModelId() {
    for (uint32_t i = 0; i < gAvailableModelIDs.size(); i++) {
        if (gAvailableModelIDs[i] == model_id_free)
            return i;
    }
    throw std::runtime_error("no id available for model");
}

namespace components {
    Renderable::Renderable(const std::string& n, Mesh& mesh)
        :Transform(n), mMesh(mesh), mModelMatrixUniform(*this, GetAvailableModelId())
    {
        //take the position
        gAvailableModelIDs[mModelMatrixUniform.mModelId] = model_id_used;
        mPhongProperties = std::make_unique<RenderablePhongProperties>(*this, mModelMatrixUniform.mModelId);
    }
    Renderable::~Renderable()
    {
        //release the position
        gAvailableModelIDs[mModelMatrixUniform.mModelId] = model_id_free;
    }
    void Renderable::SetUniforms(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        this->mPhongProperties->SetUniform(currentFrame, pipeline, cmdBuffer);
        this->mModelMatrixUniform.SetUniform(currentFrame, pipeline, cmdBuffer);
    }
    void Renderable::AdvanceAnimation(float deltaTime)
    {
        if (mAnimations.size() > 0) {
            std::shared_ptr<Animation> current = mAnimations.front();
            if (current->IsFinished()) {
                current->OnEnd();
                mAnimations.pop();
                if (mAnimations.size() > 0) {
                    current = mAnimations.front();
                    current->OnStart();
                }
            }
            else {
                current->Advance(deltaTime);
            }
        }
    }
    PhongModelMatrixUniform::PhongModelMatrixUniform(Renderable& owner, uint32_t mModelId):
        components::ModelMatrixUniform(GetAvailableModelId()), mOwner(owner)
    {
    }
    void PhongModelMatrixUniform::SetUniform(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        //update model matrix
        glm::mat4 rotationMatrix = glm::toMat4(mOwner.mOrientation);
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mOwner.mPosition);
        glm::mat4 model = translationMatrix * rotationMatrix;
        mModelData.model = model;

        ModelMatrixUniform::SetUniform(currentFrame, pipeline, cmdBuffer);
    }
   
    RenderablePhongProperties::RenderablePhongProperties(Renderable& owner, uint32_t mModelId)
        :components::PhongPropertiesUniform(mModelId), mOwner(owner)
    {
    }

    void RenderablePhongProperties::SetUniform(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        mPhongData.ambientColorAndStrength = glm::vec4(this->mAmbientColor, this->mAmbientStrength);
        mPhongData.specularStrength = glm::vec4(this->mSpecularStrength);
        PhongPropertiesUniform::SetUniform(currentFrame, pipeline, cmdBuffer);
    }

}