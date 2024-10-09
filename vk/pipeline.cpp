#include "pipeline.h"
#include "device.h"
#include <cassert>
#include "render_pass.h"
#include <fstream>
#include "io/asset-paths.h"
#include "utils/concatenate.h"
#include "debug_utils.h"

vk::Pipeline::Pipeline(const std::string name, const RenderPass& rp):
    mName(name), mHash(utils::Hash(name)), mRenderPass(rp)
{
    assert(name.size() > 0); //we need a name
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
    vkDestroyDescriptorSetLayout(d, mDescriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(d, mDescriptorPool, nullptr);
}
