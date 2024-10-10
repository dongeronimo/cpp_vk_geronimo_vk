#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <io/mesh-load.h>
#include <vector>
namespace components
{
    //The vertex data structure, for a 3d vertex and it's color
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 uv0;
        glm::vec3 normal;
    };

    class Mesh {
    public:
        Mesh(io::MeshData& md);
        void Bind(VkCommandBuffer cmd)const;
        ~Mesh();
        VkDeviceSize mVertexesOffset;
        VkDeviceSize mIndexesOffset;
        uint16_t mNumberOfIndices;
    private:
        void InitGlobalMeshBufferIfNotInitialized();
        void CopyDataToGlobalBuffer(
            const std::vector<Vertex>& vertexes,
            const std::vector<uint16_t>& indices);
        /// <summary>
        /// I don't have a buffer&memory for each mesh. They are stored in one big
        /// ass buffer, and the offsets give me the location of this mesh in that
        /// big ass buffer.
        /// </summary>

    };
}