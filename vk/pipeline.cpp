#include "pipeline.h"
#include "device.h"
#include <cassert>
#include "render_pass.h"
#include <fstream>
#include "io/asset-paths.h"
#include "utils/concatenate.h"
#include "debug_utils.h"
#include "components/renderable.h"
#include "components/mesh.h"

vk::Pipeline::Pipeline(const std::string name, const RenderPass& rp):
    mName(name), mHash(utils::Hash(name)), mRenderPass(rp)
{
    assert(name.size() > 0); //we need a name
    mRenderables.resize(MAX_NUMBER_OF_OBJS, nullptr);
}
void vk::Pipeline::Bind(VkCommandBuffer buffer, uint32_t currentFrame)
{
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
}
VkShaderModule vk::LoadShaderModule(VkDevice device, const std::string& name)
{
    auto path = io::CalculatePathForShader(name);
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("failed to open file!");
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> binaryCode(fileSize);
    file.seekg(0);
    file.read(binaryCode.data(), fileSize);
    file.close();
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = binaryCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(binaryCode.data());
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        auto errMsg = Concatenate("failed to create shader module: ", name);
        throw std::runtime_error(errMsg);
    }
    SET_NAME(shaderModule, VK_OBJECT_TYPE_SHADER_MODULE, name.c_str());
    return shaderModule;
}
vk::Pipeline::~Pipeline()
{
    auto d = Device::gDevice->GetDevice();
    vkDestroyPipeline(d, mPipeline, nullptr);
    vkDestroyPipelineLayout(d, mPipelineLayout, nullptr);
    for (auto& dsl : mDescriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(d, dsl, nullptr);
    }
    vkDestroyDescriptorPool(d, mDescriptorPool, nullptr);
}

void vk::Pipeline::DestroyPipeline()
{
    auto d = Device::gDevice->GetDevice();
    vkDestroyPipeline(d, mPipeline, nullptr);
    //vkDestroyPipelineLayout(d, mPipelineLayout, nullptr);
}

void vk::Pipeline::Draw(components::Renderable& r, VkCommandBuffer cmdBuffer, uint32_t)
{
    //I assume that all descriptor sets were bind using Uniform::Set.
    vk::SetMark({ 1.0f, 0.8f, 1.0f, 1.0f }, mName, cmdBuffer);
    r.mMesh.Bind(cmdBuffer);
    vkCmdDrawIndexed(cmdBuffer,
        r.mMesh.mNumberOfIndices,
        1,
        0,
        0,
        0);

    vk::EndMark(cmdBuffer);
}

void vk::Pipeline::AddRenderable(components::Renderable* r)
{
    mRenderables[r->mModelId] = r;
    
}

void vk::Pipeline::RemoveRenderable(components::Renderable* r)
{
    mRenderables[r->mModelId] = nullptr;
}
