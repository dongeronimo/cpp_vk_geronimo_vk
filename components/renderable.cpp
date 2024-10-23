#include "renderable.h"
#include <array>
#include <stdexcept>
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
        :Transform(n), mMesh(mesh),
        components::ModelMatrixUniform(GetAvailableModelId()), 
        mMaterial(mModelId)
    {
        //take the position
        gAvailableModelIDs[mModelId] = model_id_used;
    }
    Renderable::~Renderable()
    {
        //release the position
        gAvailableModelIDs[mModelId] = model_id_free;
    }
    void Renderable::Set(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        //update model matrix
        glm::mat4 rotationMatrix = glm::toMat4(mOrientation);
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mPosition);
        glm::mat4 model = translationMatrix * rotationMatrix;
        mModelData.model = model;

        ModelMatrixUniform::Set(currentFrame, pipeline, cmdBuffer);
    }
}