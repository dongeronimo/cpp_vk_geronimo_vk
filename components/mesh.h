#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <io/mesh-load.h>
#include <vector>
#include <mem/vma_helper.h>
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
        /*VkDeviceSize mVertexesOffset;
        VkDeviceSize mIndexesOffset;
        */
        uint16_t mNumberOfIndices;
    private:
        
        VkBuffer mMeshBuffer, mIndexBuffer;
        VmaAllocation mMeshAllocation, mIndexAllocation;
        VmaAllocationInfo mMeshAllocationInfo, mIndexAllocationInfo;

    };
}