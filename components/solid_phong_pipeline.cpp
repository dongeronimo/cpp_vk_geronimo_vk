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
        
        mDescriptorSetLayouts.push_back(cameraDescriptorSetLayout);
        mDescriptorSetLayouts.push_back(modelDescriptorSetLayout);

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
        ///Camera descriptor set
        std::vector<VkDescriptorSetLayout> cameraLayouts(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayouts[0]);
        VkDescriptorSetAllocateInfo allocInfoCamera{};
        allocInfoCamera.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfoCamera.descriptorPool = mDescriptorPool;  // Pool created earlier
        allocInfoCamera.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfoCamera.pSetLayouts = cameraLayouts.data();  // Layout for camera
        if (vkAllocateDescriptorSets(device, &allocInfoCamera, mCameraDescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate camera descriptor sets!");
        }
        ///model descriptor set
        // Allocate model descriptor sets (similar to camera)
        std::vector<VkDescriptorSetLayout> layouts (MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayouts[1]);
        VkDescriptorSetAllocateInfo allocInfoModel{};
        allocInfoModel.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfoModel.descriptorPool = mDescriptorPool;  // Same pool
        allocInfoModel.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfoModel.pSetLayouts = layouts.data();  // Layout for model

        if (vkAllocateDescriptorSets(device, &allocInfoModel, mModelDescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate model descriptor sets!");
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

    void CameraUniform::Set(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer) 
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
            0, //set 0 
            1,
            &phong.mCameraDescriptorSet[currentFrame], 0, nullptr);
    }

    void ModelMatrixUniform::Set(uint32_t currentFrame, 
        const vk::Pipeline& pipeline, 
        VkCommandBuffer cmdBuffer)
    {
        assert(mModelId != UINT32_MAX);
        static VkPhysicalDevice physicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
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
            1, //set 0 
            1,
            &phong.mModelDescriptorSet[currentFrame], 
            1, //number of dynamic offsets
            &offset); //dynamic offset
    }

}

//VkDescriptorPool CreateDescriptorPool(const std::string& name) {
//    const auto device = vk::Device::gDevice->GetDevice();
//    std::array<VkDescriptorPoolSize, 1> poolSizes = {};
//    //Camera
//    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    poolSizes[0].descriptorCount = 1 * MAX_FRAMES_IN_FLIGHT; 
//  
//    VkDescriptorPoolCreateInfo poolInfo = {};
//    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//    poolInfo.pPoolSizes = poolSizes.data();
//    poolInfo.maxSets = (1) * MAX_FRAMES_IN_FLIGHT; 
//
//    VkDescriptorPool descriptorPool;
//    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create descriptor pool!");
//    }
//    auto _n = Concatenate(name, "DescriptorPool");
//    SET_NAME(descriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, _n.c_str());
//}

//////////////////////////////////OLD/////////////////////////////
//VkDescriptorSetLayoutBinding CameraLayoutBinding();
//VkDescriptorSetLayoutBinding ModelLayoutBinding();
//VkDescriptorSetLayoutBinding LightSpaceLayoutBinding();
//VkDescriptorSetLayoutBinding MaterialLayoutBinding();
//VkDescriptorSetLayoutBinding LightPositionLayoutBinding();
//VkDescriptorSetLayoutBinding ShadowMapLayoutBinding();
//VkDescriptorSetLayout CreateDescriptorSetLayout();
//VkDescriptorPool CreateDescriptorPool();
//
//components::SolidPhongPipeline::SolidPhongPipeline()
//    :vk::Pipeline("SolidPhongPipeline"),
//    mCameraBuffer(
//        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
//        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//        vk::Device::gDevice->GetDevice()),
//    mLightSpaceMatrixBuffer(
//        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//        vk::Device::gDevice->GetDevice()),
//    mLightPositionBuffer(
//        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//        vk::Device::gDevice->GetDevice()
//    ),
//    mModelUniformBuffer(
//        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//        vk::Device::gDevice->GetDevice(),
//        vk::Instance::gInstance->GetPhysicalDevice()
//    ),
//    mMaterialUniformBuffer(
//        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//        vk::Device::gDevice->GetDevice(),
//        vk::Instance::gInstance->GetPhysicalDevice()
//    )
//{
//    const auto device = vk::Device::gDevice->GetDevice();
//    mDescriptorSetLayout = CreateDescriptorSetLayout();
//    const auto dsl_name = Concatenate(mName, "DescriptorSetLayout");
//    SET_NAME(mDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, dsl_name.c_str());
//    mDescriptorPool = CreateDescriptorPool();
//    const auto dp_name = Concatenate(mName, "DescriptorPool");
//    SET_NAME(mDescriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, dp_name.c_str());
//    CreateDescriptorSets();
//}
//
//components::SolidPhongPipeline::~SolidPhongPipeline()
//{
//}
//void components::SolidPhongPipeline::Bind()
//{

//}
//void components::SolidPhongPipeline::CreateDescriptorSets()
//{
//    const auto device = vk::Device::gDevice->GetDevice();
//    ring_buffer_t<VkDescriptorSetLayout> layouts;
//    for (int i = 0; i < layouts.size(); i++) 
//        layouts[i] = mDescriptorSetLayout;
//    VkDescriptorSetAllocateInfo allocInfo = {};
//    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//    allocInfo.descriptorPool = mDescriptorPool;
//    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
//    allocInfo.pSetLayouts = layouts.data();
//    if (vkAllocateDescriptorSets(device, &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) {
//        throw std::runtime_error("failed to allocate descriptor sets!");
//    }
//    for (int i = 0; i < mDescriptorSets.size(); i++)
//    {
//        const auto name_ds = Concatenate(mName, "DescriptorSet", i);
//        SET_NAME(mDescriptorSets[i], VK_OBJECT_TYPE_DESCRIPTOR_SET, name_ds.c_str());
//        VkDescriptorBufferInfo cameraBufferInfo = {};
//        cameraBufferInfo.buffer = mCameraBuffer.GetBuffer()[i];
//        cameraBufferInfo.offset = 0;
//        cameraBufferInfo.range = sizeof(CameraUniformBuffer);
//
//        VkDescriptorBufferInfo modelBufferInfo = {};
//        modelBufferInfo.buffer = mModelUniformBuffer.GetBuffer()[i];
//        modelBufferInfo.offset = i * mModelUniformBuffer.DynamicAlignment() * mModelUniformBuffer.Items(); // Dynamic offset for each frame
//        modelBufferInfo.range = sizeof(ModelUniformBuffer);
//
//        VkDescriptorBufferInfo lightSpaceBufferInfo = {};
//        lightSpaceBufferInfo.buffer = mLightSpaceMatrixBuffer.GetBuffer()[i];
//        lightSpaceBufferInfo.offset = 0;
//        lightSpaceBufferInfo.range = sizeof(LightSpaceMatrixUniformBuffer);
//
//        VkDescriptorBufferInfo materialBufferInfo = {};
//        materialBufferInfo.buffer = mMaterialUniformBuffer.GetBuffer()[i];
//        materialBufferInfo.offset = i * mMaterialUniformBuffer.DynamicAlignment() * mMaterialUniformBuffer.Items(); // Dynamic offset for each frame
//        materialBufferInfo.range = sizeof(MaterialUniformBuffer);
//
//        VkDescriptorBufferInfo lightPositionBufferInfo = {};
//        lightPositionBufferInfo.buffer = mLightPositionBuffer.GetBuffer()[i];
//        lightPositionBufferInfo.offset = 0;
//        lightPositionBufferInfo.range = sizeof(LightPositionUniformBuffer);
//
//        VkDescriptorImageInfo shadowMapInfo = {};
//        shadowMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        shadowMapInfo.imageView = shadowMapImageView;
//        shadowMapInfo.sampler = shadowMapSampler;
//
//        std::array<VkWriteDescriptorSet, 6> descriptorWrites = {};
//
//        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[0].dstSet = mDescriptorSets[i];
//        descriptorWrites[0].dstBinding = 0;
//        descriptorWrites[0].dstArrayElement = 0;
//        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//        descriptorWrites[0].descriptorCount = 1;
//        descriptorWrites[0].pBufferInfo = &cameraBufferInfo;
//
//        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[1].dstSet = mDescriptorSets[i];
//        descriptorWrites[1].dstBinding = 1;
//        descriptorWrites[1].dstArrayElement = 0;
//        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//        descriptorWrites[1].descriptorCount = 1;
//        descriptorWrites[1].pBufferInfo = &modelBufferInfo;
//
//        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[2].dstSet = mDescriptorSets[i];
//        descriptorWrites[2].dstBinding = 2;
//        descriptorWrites[2].dstArrayElement = 0;
//        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//        descriptorWrites[2].descriptorCount = 1;
//        descriptorWrites[2].pBufferInfo = &lightSpaceBufferInfo;
//
//        descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[3].dstSet = mDescriptorSets[i];
//        descriptorWrites[3].dstBinding = 3;
//        descriptorWrites[3].dstArrayElement = 0;
//        descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//        descriptorWrites[3].descriptorCount = 1;
//        descriptorWrites[3].pBufferInfo = &materialBufferInfo;
//
//        descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[4].dstSet = mDescriptorSets[i];
//        descriptorWrites[4].dstBinding = 4;
//        descriptorWrites[4].dstArrayElement = 0;
//        descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//        descriptorWrites[4].descriptorCount = 1;
//        descriptorWrites[4].pBufferInfo = &lightPositionBufferInfo;
//
//        descriptorWrites[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[5].dstSet = mDescriptorSets[i];
//        descriptorWrites[5].dstBinding = 5;
//        descriptorWrites[5].dstArrayElement = 0;
//        descriptorWrites[5].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//        descriptorWrites[5].descriptorCount = 1;
//        descriptorWrites[5].pImageInfo = &shadowMapInfo;
//
//        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
//    }
//}
//
//VkDescriptorPool CreateDescriptorPool() {
//    const auto device = vk::Device::gDevice->GetDevice();
//    std::array<VkDescriptorPoolSize, 4> poolSizes = {};
//    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    poolSizes[0].descriptorCount = 1 * MAX_FRAMES_IN_FLIGHT; // Camera + LightPosition + Model
//    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    poolSizes[1].descriptorCount = 1 * MAX_FRAMES_IN_FLIGHT; // ShadowMap
//    poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//    poolSizes[2].descriptorCount = MAX_NUMBER_OF_OBJS * MAX_FRAMES_IN_FLIGHT; // Material
//    poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//    poolSizes[3].descriptorCount = MAX_NUMBER_OF_OBJS * MAX_FRAMES_IN_FLIGHT; // Material
//
//    VkDescriptorPoolCreateInfo poolInfo = {};
//    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//    poolInfo.pPoolSizes = poolSizes.data();
//    poolInfo.maxSets = (MAX_NUMBER_OF_OBJS + 1 + 1 + 1) * MAX_FRAMES_IN_FLIGHT; // 1000 objects + Camera + LightPosition + ShadowMap
//
//    VkDescriptorPool descriptorPool;
//    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create descriptor pool!");
//    }
//}
//VkDescriptorSetLayout CreateDescriptorSetLayout() {
//    const auto device = vk::Device::gDevice->GetDevice();
//    const std::array<VkDescriptorSetLayoutBinding, 6> bindings = {
//        CameraLayoutBinding(),
//        //ModelLayoutBinding(),
//        //LightSpaceLayoutBinding(),
//        //MaterialLayoutBinding(),
//        //LightPositionLayoutBinding(),
//        //ShadowMapLayoutBinding(),
//    };
//    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
//    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
//    layoutInfo.pBindings = bindings.data();
//    VkDescriptorSetLayout descriptorSetLayout;
//    vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
//
//    return descriptorSetLayout;
//}
//VkDescriptorSetLayoutBinding CameraLayoutBinding() {
//    VkDescriptorSetLayoutBinding cameraLayoutBinding = {};
//    cameraLayoutBinding.binding = 0;
//    cameraLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    cameraLayoutBinding.descriptorCount = 1;
//    cameraLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
//    cameraLayoutBinding.pImmutableSamplers = nullptr;
//    return cameraLayoutBinding;
//}
//
//VkDescriptorSetLayoutBinding ModelLayoutBinding() {
//    VkDescriptorSetLayoutBinding modelLayoutBinding = {};
//    modelLayoutBinding.binding = 1;
//    modelLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//    modelLayoutBinding.descriptorCount = 1;
//    modelLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//    modelLayoutBinding.pImmutableSamplers = nullptr;
//    return modelLayoutBinding;
//}
//
//VkDescriptorSetLayoutBinding LightSpaceLayoutBinding() {
//    VkDescriptorSetLayoutBinding lightSpaceLayoutBinding = {};
//    lightSpaceLayoutBinding.binding = 2;
//    lightSpaceLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    lightSpaceLayoutBinding.descriptorCount = 1;
//    lightSpaceLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//    lightSpaceLayoutBinding.pImmutableSamplers = nullptr;
//    return lightSpaceLayoutBinding;
//}
//
//VkDescriptorSetLayoutBinding MaterialLayoutBinding() {
//    VkDescriptorSetLayoutBinding materialLayoutBinding = {};
//    materialLayoutBinding.binding = 3;
//    materialLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//    materialLayoutBinding.descriptorCount = 1;
//    materialLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//    materialLayoutBinding.pImmutableSamplers = nullptr;
//    return materialLayoutBinding;
//}
//
//VkDescriptorSetLayoutBinding LightPositionLayoutBinding() {
//    VkDescriptorSetLayoutBinding lightPositionLayoutBinding = {};
//    lightPositionLayoutBinding.binding = 4;
//    lightPositionLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    lightPositionLayoutBinding.descriptorCount = 1;
//    lightPositionLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//    lightPositionLayoutBinding.pImmutableSamplers = nullptr;
//    return lightPositionLayoutBinding;
//}
//
//VkDescriptorSetLayoutBinding ShadowMapLayoutBinding() {
//    VkDescriptorSetLayoutBinding shadowMapLayoutBinding = {};
//    shadowMapLayoutBinding.binding = 5;
//    shadowMapLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    shadowMapLayoutBinding.descriptorCount = 1;
//    shadowMapLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//    shadowMapLayoutBinding.pImmutableSamplers = nullptr;
//    return shadowMapLayoutBinding;
//}
