#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
//#include "entities/mesh-data.h"
namespace io {
    struct MeshData {
        std::string name;
        std::vector<glm::vec3> vertices;
        std::vector<uint16_t> indices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uv0s;
    };
    /// <summary>
    /// Remember that a file can have many meshes.
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    std::vector<io::MeshData> LoadMeshes(
        const std::string& file
    );
}