#include "shadow_map_pipeline.h"
#include <vk/device.h>
#include <vk/render_pass.h>
#include "uniform_buffers.h"
#include <stdexcept>
#include <utils/concatenate.h>
#include <vk/debug_utils.h>
#include <utils/vk_utils.h>
#include "solid_phong_pipeline.h"
#include <vk/instance.h>
std::string smp_name = "DirectionalShadowMapPipeline";
namespace components {
    DirectionalLightShadowMapPipeline::DirectionalLightShadowMapPipeline(const vk::RenderPass& rp)
        :vk::Pipeline(smp_name, rp)
    {
        const auto device = vk::Device::gDevice->GetDevice();
        mVertexShader = vk::LoadShaderModule(device, "directional_shadow_map.vert.spv");
        mFragmentShader = vk::LoadShaderModule(device, "directional_shadow_map.frag.spv");
        CreateDescriptorSetLayout();
        CreateModelBuffer();
        CreateDescriptorPool();
        CreateDescriptorSet();


        CreatePipelineLayout();
        Recreate();
    }

    DirectionalLightShadowMapPipeline::~DirectionalLightShadowMapPipeline()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        vkDestroyShaderModule(device, mVertexShader, nullptr);
        vkDestroyShaderModule(device, mFragmentShader, nullptr);
    }

    void DirectionalLightShadowMapPipeline::Recreate()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        auto shaderStages = CreateShaderStage(mVertexShader, mFragmentShader);
        auto attributes = DefaultAttributeDescription();
        auto bindings = DefaultBindingDescription();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindings;
        vertexInputInfo.vertexAttributeDescriptionCount = attributes.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributes.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(mRenderPass.GetExtent().width);
        viewport.height = static_cast<float>(mRenderPass.GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = mRenderPass.GetExtent();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.layout = mPipelineLayout;
        pipelineInfo.renderPass = mRenderPass.GetRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
    }

    void DirectionalLightShadowMapPipeline::Bind(VkCommandBuffer buffer, uint32_t currentFrame)
    {
        vk::Pipeline::Bind(buffer, currentFrame);

    }

    void DirectionalLightShadowMapPipeline::SetLightMatrix(VkCommandBuffer buffer, LightSpaceMatrixUniformBuffer& lm)
    {
        vkCmdPushConstants(buffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(LightSpaceMatrixUniformBuffer), &lm);
    }

    void DirectionalLightShadowMapPipeline::CreateDescriptorSetLayout()
    {
        VkDescriptorSetLayout modelDescriptorSetLayout = vk::ModelMatrixDescriptorSetLayout();
        auto n2 = Concatenate(mName, "ModelDescriptorSetLayout");
        SET_NAME(modelDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n2.c_str());
        mDescriptorSetLayouts.push_back(modelDescriptorSetLayout);
    }

    void DirectionalLightShadowMapPipeline::CreateDescriptorPool()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        std::array<VkDescriptorPoolSize, 1> poolSizes;
        // Model - dynamic buffer, one per frame
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT; // Adjust this for the number of models
        /////Create the descriptor pool/////
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 2 * MAX_FRAMES_IN_FLIGHT; // 2 descriptor sets per frame
        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
        auto n = Concatenate(mName, "DescriptorPool");
        SET_NAME(mDescriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, n.c_str());
    }

    void DirectionalLightShadowMapPipeline::CreateModelBuffer()
    {
        /////Create the model buffer, one for each frame, with size for 1000 objs/////
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            utils::CreateAlignedBuffer(sizeof(CameraUniformBuffer),
                MAX_NUMBER_OF_OBJS,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                mModelBuffer[i], mModelBufferMemory[i]);
        }
    }

    void DirectionalLightShadowMapPipeline::CreatePipelineLayout()
    {
        VkPushConstantRange pushConstantRange;
        pushConstantRange.offset = 0;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.size = sizeof(components::LightSpaceMatrixUniformBuffer);
        const auto device = vk::Device::gDevice->GetDevice();
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        pipelineLayoutInfo.setLayoutCount = mDescriptorSetLayouts.size();                                     
        pipelineLayoutInfo.pSetLayouts = mDescriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo,
            nullptr, &mPipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }
    }

    void DirectionalLightShadowMapPipeline::CreateDescriptorSet()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        ///model descriptor set
        // Allocate model descriptor sets (similar to camera)
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayouts[0]);
        VkDescriptorSetAllocateInfo allocInfoModel{};
        allocInfoModel.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfoModel.descriptorPool = mDescriptorPool;  // Same pool
        allocInfoModel.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfoModel.pSetLayouts = layouts.data();  // Layout for model

        if (vkAllocateDescriptorSets(device, &allocInfoModel, mModelDescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate model descriptor sets!");
        }
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo modelBufferInfo{};
            modelBufferInfo.buffer = mModelBuffer[i];  // Your model buffer (dynamic)
            modelBufferInfo.offset = 0;  // Adjust offset per object/model
            modelBufferInfo.range = utils::AlignedSize(sizeof(ModelUniformBuffer), MAX_NUMBER_OF_OBJS, 
                vk::Instance::gInstance->GetPhysicalDevice());  // Size of model data
            VkWriteDescriptorSet modelDescriptorWrite{};
            modelDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            modelDescriptorWrite.dstSet = mModelDescriptorSet[i];  // Descriptor set to update
            modelDescriptorWrite.dstBinding = 0;  // Binding 0 in the shader (model)
            modelDescriptorWrite.dstArrayElement = 0;
            modelDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            modelDescriptorWrite.descriptorCount = 1;  // One buffer
            modelDescriptorWrite.pBufferInfo = &modelBufferInfo;  // Buffer info

            std::array<VkWriteDescriptorSet, 1> descriptorWrites = { modelDescriptorWrite };
            // Perform the update
            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
            auto _n = Concatenate(mName, "ModelDescriptorSet", i);
            SET_NAME(mModelDescriptorSet[i], VK_OBJECT_TYPE_DESCRIPTOR_SET, _n.c_str());
        }
    }

}
