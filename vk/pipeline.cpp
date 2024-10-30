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
    mRenderables.resize(MAX_NUMBER_OF_OBJS);
    assert(name.size() > 0); //we need a name
}

void vk::Pipeline::Bind(VkCommandBuffer buffer, uint32_t currentFrame)
{
    vk::SetMark({ 0.1f, 0.4f, 0.3f, 1.0f }, mName, buffer);
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
}
void vk::Pipeline::Unbind(VkCommandBuffer buffer)
{
    vk::EndMark(buffer);
}
VkDescriptorSetLayout vk::ModelMatrixDescriptorSetLayout()
{
    const auto device = vk::Device::gDevice->GetDevice();
    VkDescriptorSetLayoutBinding modelBindings;
    modelBindings.binding = 0; // Matches binding 0 in the shader
    modelBindings.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    modelBindings.descriptorCount = 1;
    modelBindings.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    modelBindings.pImmutableSamplers = nullptr; // Not used
    VkDescriptorSetLayoutCreateInfo modelLayoutInfo{};
    modelLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    modelLayoutInfo.bindingCount = 1; // Only one binding, for the Camera uniform buffer
    modelLayoutInfo.pBindings = &modelBindings;
    VkDescriptorSetLayout modelDescriptorSetLayout;
    if (vkCreateDescriptorSetLayout(device, &modelLayoutInfo, nullptr, &modelDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
    return modelDescriptorSetLayout;
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

void vk::Pipeline::Draw(components::Renderable& r, VkCommandBuffer cmdBuffer)
{
    //I assume that all descriptor sets were bind using Uniform::Set.
    vk::SetMark({ 1.0f, 0.8f, 1.0f, 1.0f }, r.mName, cmdBuffer);
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
    mRenderables[r->GetModelId()] = r;
}

void vk::Pipeline::RemoveRenderable(components::Renderable* r)
{
    mRenderables[r->GetModelId()] = nullptr;
}

std::vector<VkPipelineShaderStageCreateInfo> vk::Pipeline::CreateShaderStage(VkShaderModule vert, VkShaderModule frag)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vert;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = frag;
    fragShaderStageInfo.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };
    return shaderStages;
}

std::vector<VkVertexInputAttributeDescription> vk::Pipeline::DefaultAttributeDescription()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
    //inPosition
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(components::Vertex, pos);
    //inNormal
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 1;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(components::Vertex, normal);
    //inUV0
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 2;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(components::Vertex, uv0);

    return attributeDescriptions;
}

VkVertexInputBindingDescription vk::Pipeline::DefaultBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(components::Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}
