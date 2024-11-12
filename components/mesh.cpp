#include "mesh.h"
#include <cassert>
#include <vk/device.h>
#include <stdexcept>
#include <utils/vk_utils.h>
#include <vk/debug_utils.h>
//#define _256mb 256 * 1024 * 1024
//static VkBuffer gMeshBuffer = VK_NULL_HANDLE;
//static VkDeviceMemory gMeshMemory = VK_NULL_HANDLE;
//uint32_t meshCounter = 0;
//uintptr_t gMemoryCursor = 0;

namespace components {
    Mesh::Mesh(io::MeshData& md)
    {
        VkPhysicalDevice physicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
        //////1) create the local buffer and copy the content to it
        std::vector<components::Vertex> vertexes(md.vertices.size());
        for (auto i = 0; i < md.vertices.size(); i++) {
            vertexes[i].pos = md.vertices[i];
            vertexes[i].uv0 = md.uv0s[i];
            vertexes[i].normal = md.normals[i];
        }
        //now the data is in a good format for the transfer, we can memcpy from meshData
        const size_t meshSizeInBytes = sizeof(vertexes[0]) * vertexes.size();
        auto& helper = mem::VmaHelper::GetInstance();
        VkBuffer meshStagingBuffer;
        VmaAllocation meshStagingAllocation;
        VmaAllocationInfo meshStagingAllocationInfo;
        helper.CreateAlignedBuffer(meshSizeInBytes,
            utils::GetMinAlignment(physicalDevice),
            1,
            meshStagingBuffer,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            meshStagingAllocation,
            meshStagingAllocationInfo);
        helper.CopyToAllocation(vertexes.data(),
            meshStagingAllocation, 0, meshSizeInBytes);
        const size_t indexSizeInBytes = md.indices.size() * sizeof(uint16_t);
        VkBuffer indexStagingBuffer;
        VmaAllocation indexStagingBufferAllocation;
        VmaAllocationInfo indexStagingBufferAllocationInfo;
        helper.CreateAlignedBuffer(indexSizeInBytes,
            utils::GetMinAlignment(physicalDevice),
            1,
            indexStagingBuffer,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            indexStagingBufferAllocation,
            indexStagingBufferAllocationInfo);
        helper.CopyToAllocation(md.indices.data(),
            indexStagingBufferAllocation, 0, indexSizeInBytes);
        //////2) create the gpu buffer
        helper.CreateBufferInGPU(meshSizeInBytes,
            1,
            mMeshBuffer,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            mMeshAllocation,
            mMeshAllocationInfo);
        helper.CreateBufferInGPU(indexSizeInBytes,
            1,
            mIndexBuffer,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            mIndexAllocation,
            mIndexAllocationInfo);
        //////3) copy from the local buffer to the gpu buffer
        VkCommandBuffer vbCopyCommandBuffer = vk::Device::gDevice->CreateCommandBuffer(
            "vertex buffer copy command buffer");
        vk::Device::gDevice->BeginRecordingCommands(vbCopyCommandBuffer);
        vk::Device::gDevice->CopyBuffer(0, 0, meshSizeInBytes,
            vbCopyCommandBuffer, meshStagingBuffer, mMeshBuffer);
        vk::Device::gDevice->CopyBuffer(0, 0, indexSizeInBytes,
            vbCopyCommandBuffer, indexStagingBuffer, mIndexBuffer);
        vk::Device::gDevice->SubmitAndFinishCommands(vbCopyCommandBuffer);
        //////4) delete local buffer
        vkDestroyBuffer(vk::Device::gDevice->GetDevice(), meshStagingBuffer, nullptr);
        vkDestroyBuffer(vk::Device::gDevice->GetDevice(), indexStagingBuffer, nullptr);
        helper.FreeMemory(meshStagingAllocation);
        helper.FreeMemory(indexStagingBufferAllocation);
        mNumberOfIndices = md.indices.size();
    }
    void Mesh::Bind(VkCommandBuffer cmd)const
    {
        VkDeviceSize meshOffsets{ 0 };
        vkCmdBindVertexBuffers(cmd, 0, 1, &mMeshBuffer, &meshOffsets);
        vkCmdBindIndexBuffer(cmd, mIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
    }
    Mesh::~Mesh() {
        auto device = vk::Device::gDevice->GetDevice();
        auto& helper = mem::VmaHelper::GetInstance();
        vkDestroyBuffer(device, mMeshBuffer, nullptr);
        vkDestroyBuffer(device, mIndexBuffer, nullptr);
        helper.FreeMemory(mMeshAllocation);
        helper.FreeMemory(mIndexAllocation);
    }

}
