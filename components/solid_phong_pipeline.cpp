#include "solid_phong_pipeline.h"
#include <array>
#include <vk/device.h>
#include <stdexcept>
#include <vk/debug_utils.h>
#include <utils/concatenate.h>
#include <utils/vk_utils.h>
#include <vk/instance.h>
#include <vk/render_pass.h>
#include "mesh.h"
#include "shadow_map_pipeline.h"
namespace components
{
    void SolidPhongPipeline::Recreate()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        ////Create the pipeline////
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = mVertexShader;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = mFragmentShader;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        auto attributes = AttributeDescription();
        auto bindings = BindingDescription();
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
        //VkPipelineDynamicStateCreateInfo dynamicState{};
        //dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        //dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        //dynamicState.pDynamicStates = dynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        //pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.layout = mPipelineLayout;
        pipelineInfo.renderPass = mRenderPass.GetRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
    }
    SolidPhongPipeline::SolidPhongPipeline(const vk::RenderPass& rp)
        :vk::Pipeline("SolidPhongPipeline", rp)
    {
        const auto device = vk::Device::gDevice->GetDevice();
        CreateDescriptorSetLayout();
        CreateDescriptorPool();
        CreateCameraBuffer();
        CreateModelBuffer();
        CreateDescriptorSet();
        CreatePipelineLayout();
        ////Load the shaders////
        mVertexShader = vk::LoadShaderModule(device, "phong.vert.spv");
        mFragmentShader = vk::LoadShaderModule(device, "phong.frag.spv");
        Recreate();
    }

    SolidPhongPipeline::~SolidPhongPipeline()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        vkDestroyShaderModule(device, mVertexShader, nullptr);
        vkDestroyShaderModule(device, mFragmentShader, nullptr);
        vkDestroyDescriptorPool(device, mDescriptorPool, nullptr);
        for (auto& m : mCameraBuffer) {
            vkDestroyBuffer(device, m, nullptr);
        }
        for (auto& m : mCameraBufferMemory) {
            vkFreeMemory(device, m, nullptr);
        }
    }
    void SolidPhongPipeline::Bind(VkCommandBuffer buffer, uint32_t currentFrame)
    {
        Pipeline::Bind(buffer, currentFrame);
    }



    std::vector<VkVertexInputAttributeDescription> SolidPhongPipeline::AttributeDescription()
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

    VkVertexInputBindingDescription SolidPhongPipeline::BindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(components::Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    void SolidPhongPipeline::CreateDescriptorSetLayout()
    {
        const auto device = vk::Device::gDevice->GetDevice();

        VkDescriptorSetLayoutBinding cameraBindings;
        cameraBindings.binding = 0; // Matches binding 0 in the shader
        cameraBindings.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        cameraBindings.descriptorCount = 1;
        cameraBindings.stageFlags = VK_SHADER_STAGE_VERTEX_BIT| VK_SHADER_STAGE_FRAGMENT_BIT;
        cameraBindings.pImmutableSamplers = nullptr; // Not used
        VkDescriptorSetLayoutCreateInfo cameralayoutInfo{};
        cameralayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        cameralayoutInfo.bindingCount = 1; // Only one binding, for the Camera uniform buffer
        cameralayoutInfo.pBindings = &cameraBindings;
        VkDescriptorSetLayout cameraDescriptorSetLayout;
        if (vkCreateDescriptorSetLayout(device, &cameralayoutInfo, nullptr, &cameraDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }
        auto n1 = Concatenate(mName, "CameraDescriptorSetLayout");
        SET_NAME(cameraDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n1.c_str());

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
        auto n2 = Concatenate(mName, "ModelDescriptorSetLayout");
        SET_NAME(modelDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n2.c_str());
        
        mDescriptorSetLayouts.push_back(modelDescriptorSetLayout);
        mDescriptorSetLayouts.push_back(cameraDescriptorSetLayout);


    }

    void SolidPhongPipeline::CreateDescriptorPool()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        std::array<VkDescriptorPoolSize, 2> poolSizes;
        // Camera - uniform buffer, one per frame
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;

        // Model - dynamic buffer, one per frame
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT; // Adjust this for the number of models

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

    void SolidPhongPipeline::CreateCameraBuffer()
    {
        /////Create the camera buffer, one for each frame/////
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            utils::CreateBuffer(sizeof(CameraUniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                mCameraBuffer[i], mCameraBufferMemory[i]);
        }

    }

    void SolidPhongPipeline::CreateDescriptorSet()
    {
        const auto device = vk::Device::gDevice->GetDevice();

        ///model descriptor set
        // Allocate model descriptor sets (similar to camera)
        std::vector<VkDescriptorSetLayout> modelLayouts(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayouts[0]);
        VkDescriptorSetAllocateInfo allocInfoModel{};
        allocInfoModel.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfoModel.descriptorPool = mDescriptorPool;  // Same pool
        allocInfoModel.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfoModel.pSetLayouts = modelLayouts.data();  // Layout for model
        if (vkAllocateDescriptorSets(device, &allocInfoModel, mModelDescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate model descriptor sets!");
        }
        ///Camera descriptor set
        std::vector<VkDescriptorSetLayout> cameraLayouts(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayouts[1]);
        VkDescriptorSetAllocateInfo allocInfoCamera{};
        allocInfoCamera.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfoCamera.descriptorPool = mDescriptorPool;  // Pool created earlier
        allocInfoCamera.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfoCamera.pSetLayouts = cameraLayouts.data();  // Layout for camera
        if (vkAllocateDescriptorSets(device, &allocInfoCamera, mCameraDescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate camera descriptor sets!");
        }


        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo cameraBufferInfo{};
            cameraBufferInfo.buffer = mCameraBuffer[i];  // Your camera buffer for each frame
            cameraBufferInfo.offset = 0;
            cameraBufferInfo.range = sizeof(CameraUniformBuffer);  // Size of the camera data
            VkWriteDescriptorSet cameraDescriptorWrite{};
            cameraDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            cameraDescriptorWrite.dstSet = mCameraDescriptorSet[i];  // Descriptor set to update
            cameraDescriptorWrite.dstBinding = 0;  // Binding 0 in the shader (camera)
            cameraDescriptorWrite.dstArrayElement = 0;  // No array elements
            cameraDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            cameraDescriptorWrite.descriptorCount = 1;  // One buffer
            cameraDescriptorWrite.pBufferInfo = &cameraBufferInfo;  // Buffer info


            VkDescriptorBufferInfo modelBufferInfo{};
            modelBufferInfo.buffer = mModelBuffer[i];  // Your model buffer (dynamic)
            modelBufferInfo.offset = 0;  // Adjust offset per object/model
            modelBufferInfo.range = utils::AlignedSize(sizeof(ModelUniformBuffer), MAX_NUMBER_OF_OBJS, vk::Instance::gInstance->GetPhysicalDevice());  // Size of model data
            VkWriteDescriptorSet modelDescriptorWrite{};
            modelDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            modelDescriptorWrite.dstSet = mModelDescriptorSet[i];  // Descriptor set to update
            modelDescriptorWrite.dstBinding = 0;  // Binding 0 in the shader (model)
            modelDescriptorWrite.dstArrayElement = 0;
            modelDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            modelDescriptorWrite.descriptorCount = 1;  // One buffer
            modelDescriptorWrite.pBufferInfo = &modelBufferInfo;  // Buffer info

            std::array<VkWriteDescriptorSet, 2> descriptorWrites = { cameraDescriptorWrite, modelDescriptorWrite };
            // Perform the update
            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void SolidPhongPipeline::CreatePipelineLayout()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        std::vector<VkDescriptorSetLayout> layouts = mDescriptorSetLayouts;
        ////Create the pipeline layout////
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = layouts.size(); // One descriptor set layout
        pipelineLayoutInfo.pSetLayouts = layouts.data(); // The layout created earlier
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, 
            nullptr, &mPipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }

    }

    void SolidPhongPipeline::CreateModelBuffer()
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

    void CameraUniform::SetUniform(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        //get my pipeline
        const SolidPhongPipeline& phong = dynamic_cast<const SolidPhongPipeline&>(pipeline);
        //map camera data and copy to the gpu
        const auto device = vk::Device::gDevice->GetDevice();
        void* data;
        vkMapMemory(device, 
            phong.mCameraBufferMemory[currentFrame],
            0, sizeof(CameraUniformBuffer), 0, &data);
        memcpy(data, &mCameraData, sizeof(CameraUniformBuffer));
        vkUnmapMemory(device, phong.mCameraBufferMemory[currentFrame]);
        //bind camera descriptor set
        vkCmdBindDescriptorSets(cmdBuffer, 
            VK_PIPELINE_BIND_POINT_GRAPHICS, 
            phong.mPipelineLayout, 
            1, 
            1,
            &phong.mCameraDescriptorSet[currentFrame], 0, nullptr);
    }

    void ModelMatrixUniform::SetUniform(uint32_t currentFrame,
        const vk::Pipeline& pipeline, 
        VkCommandBuffer cmdBuffer)
    {
        assert(mModelId != UINT32_MAX);
        static VkPhysicalDevice physicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
        if (pipeline.mHash == utils::Hash("SolidPhongPipeline")) {
            //get my pipeline
            const SolidPhongPipeline& phong = dynamic_cast<const SolidPhongPipeline&>(pipeline);
            //map model data and copy to the gpu, mind the offsets and aligned size
            const auto device = vk::Device::gDevice->GetDevice();
            void* data;
            VkDeviceSize alignedSize = utils::AlignedSize(sizeof(ModelUniformBuffer),
                1, physicalDevice);
            uint32_t offset = alignedSize * mModelId;
            vkMapMemory(device,
                phong.mModelBufferMemory[currentFrame],
                offset, //offset
                alignedSize, //size
                0, &data);
            memcpy(data, &mModelData, sizeof(CameraUniformBuffer));
            vkUnmapMemory(device, phong.mModelBufferMemory[currentFrame]);
            //bind model descriptor set
            vkCmdBindDescriptorSets(cmdBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                phong.mPipelineLayout,
                0, //set 0 
                1,
                &phong.mModelDescriptorSet[currentFrame],
                1, //number of dynamic offsets
                &offset); //dynamic offset
        }
        if (pipeline.mHash == utils::Hash("DirectionalShadowMapPipeline"))
        {
            //get my pipeline
            const DirectionalLightShadowMapPipeline& dsm = dynamic_cast<const DirectionalLightShadowMapPipeline&>(pipeline);
            //map model data and copy to the gpu, mind the offsets and aligned size
            const auto device = vk::Device::gDevice->GetDevice();
            void* data;
            VkDeviceSize alignedSize = utils::AlignedSize(sizeof(ModelUniformBuffer),
                1, physicalDevice);
            uint32_t offset = alignedSize * mModelId;
            vkMapMemory(device,
                dsm.mModelBufferMemory[currentFrame],
                offset, //offset
                alignedSize, //size
                0, &data);
            memcpy(data, &mModelData, sizeof(CameraUniformBuffer));
            vkUnmapMemory(device, dsm.mModelBufferMemory[currentFrame]);
            //bind model descriptor set
            vkCmdBindDescriptorSets(cmdBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                dsm.mPipelineLayout,
                0, //set 0 
                1,
                &dsm.mModelDescriptorSet[currentFrame],
                1, //number of dynamic offsets
                &offset); //dynamic offset
        }
    }

}

