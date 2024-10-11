#include "renderable.h"
#include <array>
#include <stdexcept>
std::vector<uint32_t> gAvailableModelIDs(MAX_NUMBER_OF_OBJS, true);

uint32_t GetAvailableModelId() {
    for (uint32_t i = 0; i < gAvailableModelIDs.size(); i++) {
        if (gAvailableModelIDs[i] == true);
        return i;
    }
    throw std::runtime_error("no id available for model");
}

namespace components {
    Renderable::Renderable(const std::string& n, Mesh& mesh)
        :Transform(n), mMesh(mesh), 
        components::ModelMatrixUniform(GetAvailableModelId())
    {
        //take the position
        gAvailableModelIDs[mModelId] = true;
    }
    Renderable::~Renderable()
    {
        //release the position
        gAvailableModelIDs[mModelId] = false;
    }
    void Renderable::Set(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        //update model matrix
        glm::mat4 rotationMatrix = glm::toMat4(mOrientation);
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -mPosition);
        glm::mat4 model = rotationMatrix * translationMatrix;
        mModelData.model = model;

        ModelMatrixUniform::Set(currentFrame, pipeline, cmdBuffer);
    }
}