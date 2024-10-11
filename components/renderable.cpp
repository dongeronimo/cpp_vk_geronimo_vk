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


    }
}