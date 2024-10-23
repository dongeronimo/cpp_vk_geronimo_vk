#include "solid_phong_pipeline.h"
#include <array>
#include <vk/device.h>
#include <stdexcept>
#include <vk/debug_utils.h>
#include <utils/concatenate.h>
#include <utils/vk_utils.h>
#include <vk/instance.h>
#include <vk/render_pass.h>
#include <components/renderable.h>
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
        viewport.y = 0.0f;//mRenderPass.GetExtent().height;
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
    
    SolidPhongPipeline::SolidPhongPipeline(const std::string& name, const vk::RenderPass& rp, VkSampler phongTextureSampler, VkImageView textureImageView)
        :vk::Pipeline(name, rp), mPhongTextureSampler(phongTextureSampler)
    {
        const auto device = vk::Device::gDevice->GetDevice();
        CreateDescriptorSetLayout();
        CreateDescriptorPool();
        CreateCameraBuffer();
        CreateModelBuffer();
        CreateDescriptorSet(textureImageView);
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
        vkCmdBindDescriptorSets(buffer, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        mPipelineLayout, 
        0, //set 0 
        1,
        &mCameraAndPointLightDescriptorSet[currentFrame], 0, nullptr);
    }

    void SolidPhongPipeline::Draw(components::Renderable& r, VkCommandBuffer cmdBuffer, uint32_t currentFrame)
    {
        static VkPhysicalDevice physicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
        VkDeviceSize modelAlignedSize = utils::AlignedSize(sizeof(ModelUniformBuffer),
            1, physicalDevice);
        uint32_t modelOffset = modelAlignedSize * r.mModelId;
        VkDeviceSize phongAlignedSize = utils::AlignedSize(sizeof(ModelUniformBuffer),
            1, physicalDevice);
        uint32_t phongOffset = phongAlignedSize * r.mModelId;

        std::array<uint32_t, 2> offsets{ modelOffset, phongOffset };
        //bind model and phong descriptor set
        vkCmdBindDescriptorSets(cmdBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mPipelineLayout,
            1, //set 0 
            1,
            &mModelDescriptorSet[currentFrame],
            offsets.size(), //number of dynamic offsets
            offsets.data()); //dynamic offset

        //bind the texture sampler at set 2
        vkCmdBindDescriptorSets(cmdBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mPipelineLayout,
            2, //Sampler is in set 2 
            1,
            &mPhongTextureSamplerDescriptorSet, 0, nullptr);
        Pipeline::Draw(r, cmdBuffer, currentFrame);
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
    VkDescriptorSetLayoutBinding MakeBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags) {
        VkDescriptorSetLayoutBinding cameraBindings;
        cameraBindings.binding = binding; // camera is at set=0, binding = 0
        cameraBindings.descriptorType = type;
        cameraBindings.descriptorCount = 1;
        cameraBindings.stageFlags = stageFlags;
        cameraBindings.pImmutableSamplers = nullptr; // Not used
        return cameraBindings;
    }
    VkDescriptorSetLayout LayoutForCameraAndPointLights() {
        const auto device = vk::Device::gDevice->GetDevice();
        VkDescriptorSetLayoutBinding cameraBindings = MakeBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        VkDescriptorSetLayoutBinding pointLightsBindings = MakeBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        std::array<VkDescriptorSetLayoutBinding, 2> bindings{ cameraBindings, pointLightsBindings };
        VkDescriptorSetLayoutCreateInfo cameraAndLightLayoutInfo{};
        cameraAndLightLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        cameraAndLightLayoutInfo.bindingCount = bindings.size();
        cameraAndLightLayoutInfo.pBindings = bindings.data();
        VkDescriptorSetLayout cameraAndLightDescriptorSetLayout = VK_NULL_HANDLE;
        vkCreateDescriptorSetLayout(device, &cameraAndLightLayoutInfo, nullptr, &cameraAndLightDescriptorSetLayout);
        return cameraAndLightDescriptorSetLayout;
    }

    VkDescriptorSetLayout LayoutForModelMatrixAndPhongMaterial() {
        const auto device = vk::Device::gDevice->GetDevice();
        VkDescriptorSetLayoutBinding modelBinding = MakeBinding(0, 
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT);
        VkDescriptorSetLayoutBinding phongMaterialBinding = MakeBinding(1, 
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_FRAGMENT_BIT);
        std::array<VkDescriptorSetLayoutBinding, 2> bindings{ modelBinding, phongMaterialBinding };
        VkDescriptorSetLayoutCreateInfo modelMatAndMaterialLayoutInfo{};
        modelMatAndMaterialLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        modelMatAndMaterialLayoutInfo.bindingCount = bindings.size();
        modelMatAndMaterialLayoutInfo.pBindings = bindings.data();
        VkDescriptorSetLayout modelAndMaterialDescriptorSetLayout = VK_NULL_HANDLE;
        vkCreateDescriptorSetLayout(device, &modelMatAndMaterialLayoutInfo, nullptr, &modelAndMaterialDescriptorSetLayout);
        return modelAndMaterialDescriptorSetLayout;
    }

    void SolidPhongPipeline::CreateDescriptorSetLayout()
    {
        const auto device = vk::Device::gDevice->GetDevice();

        VkDescriptorSetLayout cameraAndLightDescriptorSetLayout = LayoutForCameraAndPointLights();
        auto n1 = Concatenate(mName, "CameraAndLightDescriptorSetLayout");
        SET_NAME(cameraAndLightDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n1.c_str());

        VkDescriptorSetLayout modelAndPhongDescriptorSetLayout = LayoutForModelMatrixAndPhongMaterial();
        auto n2 = Concatenate(mName, "ModelMatAndPhongDescriptorSetLayout");
        SET_NAME(modelAndPhongDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n2.c_str());
        
        VkDescriptorSetLayoutBinding textureSamplerLayoutBinding{};
        textureSamplerLayoutBinding.binding = 0;
        textureSamplerLayoutBinding.descriptorCount = 1;
        textureSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSamplerLayoutBinding.pImmutableSamplers = nullptr;
        textureSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        VkDescriptorSetLayoutCreateInfo textureSamplerLayoutInfo{};
        textureSamplerLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        textureSamplerLayoutInfo.bindingCount = 1;
        textureSamplerLayoutInfo.pBindings = &textureSamplerLayoutBinding;
        VkDescriptorSetLayout textureBindingDescriptorSetLayout;
        if (vkCreateDescriptorSetLayout(device, &textureSamplerLayoutInfo, nullptr, &textureBindingDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }
        auto n3 = Concatenate(mName, "TextureSamplerDescriptorSetLayout");
        SET_NAME(textureBindingDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n2.c_str());

        ///ORDER MATTERS! It must follow the set number used in the shader
        mDescriptorSetLayouts.push_back(cameraAndLightDescriptorSetLayout);
        mDescriptorSetLayouts.push_back(modelAndPhongDescriptorSetLayout);
        mDescriptorSetLayouts.push_back(textureBindingDescriptorSetLayout);

    }

    void SolidPhongPipeline::CreateDescriptorPool()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        std::array<VkDescriptorPoolSize, 3> poolSizes;
        // Camera and lights - uniform buffer, one per frame. Camera is at binding 0 and point lights at binding 1. 
        //they are in the same set.
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = 3*MAX_FRAMES_IN_FLIGHT;

        // Model and phong material properties - dynamic buffer, one per frame
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[1].descriptorCount = 3 * MAX_FRAMES_IN_FLIGHT; // Adjust this for the number of models

        // Texture Sampler - one sampler
        poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[2].descriptorCount = 1;
        /////Create the descriptor pool/////
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 4 * MAX_FRAMES_IN_FLIGHT + 1; // 2 descriptor sets per frame + 1 for the texture sampler
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
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            auto sz = sizeof(PointLightsUniformBuffer);
            utils::CreateAlignedBuffer(sz, 1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                mPointLightsBuffer[i], mPointLightsMemory[i]);
            //utils::CreateBuffer(sizeof(PointLightsUniformBuffer), 
            //    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            //    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            //    mPointLightsBuffer[i], mPointLightsMemory[i]);
        }
    }

    void SolidPhongPipeline::CreateDescriptorSet(VkImageView textureImageView)
    {
        const auto device = vk::Device::gDevice->GetDevice();
        ///Camera and lightning descriptor set
        std::vector<VkDescriptorSetLayout> cameraAndLightsLayout(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayouts[0]);
        VkDescriptorSetAllocateInfo allocInfoCamera{};
        allocInfoCamera.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfoCamera.descriptorPool = mDescriptorPool;  // Pool created earlier
        allocInfoCamera.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfoCamera.pSetLayouts = cameraAndLightsLayout.data();  // Layout for camera
        if (vkAllocateDescriptorSets(device, &allocInfoCamera, mCameraAndPointLightDescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate camera descriptor sets!");
        }
        ///model and phong descriptor set
        // Allocate model descriptor sets (similar to camera)
        std::vector<VkDescriptorSetLayout> modelAndPhongLayout (MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayouts[1]);
        VkDescriptorSetAllocateInfo allocInfoModel{};
        allocInfoModel.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfoModel.descriptorPool = mDescriptorPool;  // Same pool
        allocInfoModel.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfoModel.pSetLayouts = modelAndPhongLayout.data();  // Layout for model
        if (vkAllocateDescriptorSets(device, &allocInfoModel, mModelDescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate model descriptor sets!");
        }

        //texture sampler set
        VkDescriptorSetLayout textureSamplerLayout = mDescriptorSetLayouts[2];
        VkDescriptorSetAllocateInfo allocInfoTextureSampler{};
        allocInfoTextureSampler.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfoTextureSampler.descriptorPool = mDescriptorPool;
        allocInfoTextureSampler.descriptorSetCount = 1;
        allocInfoTextureSampler.pSetLayouts = &textureSamplerLayout;
        if (vkAllocateDescriptorSets(device, &allocInfoTextureSampler, &mPhongTextureSamplerDescriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate mPhongTextureSamplerDescriptorSet descriptor sets!");
        }

       


        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo cameraBufferInfo{};
            cameraBufferInfo.buffer = mCameraBuffer[i];  // Your camera buffer for each frame
            cameraBufferInfo.offset = 0;
            cameraBufferInfo.range = sizeof(CameraUniformBuffer);  // Size of the camera data
            VkWriteDescriptorSet cameraDescriptorWrite{};
            cameraDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            cameraDescriptorWrite.dstSet = mCameraAndPointLightDescriptorSet[i];  // Descriptor set to update
            cameraDescriptorWrite.dstBinding = 0;  // Binding 0 in the shader (camera and point lights)
            cameraDescriptorWrite.dstArrayElement = 0;  // No array elements
            cameraDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            cameraDescriptorWrite.descriptorCount = 1;  // One buffer
            cameraDescriptorWrite.pBufferInfo = &cameraBufferInfo;  // Buffer info

            VkDescriptorBufferInfo pointLightsBufferInfo{};
            pointLightsBufferInfo.buffer = mPointLightsBuffer[i];
            pointLightsBufferInfo.offset = 0;
            pointLightsBufferInfo.range = utils::AlignedSize(
                sizeof(PointLightsUniformBuffer), 1, vk::Instance::gInstance->GetPhysicalDevice());
            VkWriteDescriptorSet pointLightDescriptorWrite{};
            pointLightDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            pointLightDescriptorWrite.dstSet = mCameraAndPointLightDescriptorSet[i];  // Descriptor set to update
            pointLightDescriptorWrite.dstBinding = 1;  // Binding 0 in the shader (camera and point lights)
            pointLightDescriptorWrite.dstArrayElement = 0;  // No array elements
            pointLightDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            pointLightDescriptorWrite.descriptorCount = 1;  // One buffer
            pointLightDescriptorWrite.pBufferInfo = &pointLightsBufferInfo;  // Buffer info


            VkDescriptorBufferInfo modelBufferInfo{};
            modelBufferInfo.buffer = mModelBuffer[i];  // Your model buffer (dynamic)
            modelBufferInfo.offset = 0;  // Adjust offset per object/model
            modelBufferInfo.range = utils::AlignedSize(sizeof(ModelUniformBuffer), 
                1,  
                vk::Instance::gInstance->GetPhysicalDevice());  // Size of model data
            VkWriteDescriptorSet modelDescriptorWrite{};
            modelDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            modelDescriptorWrite.dstSet = mModelDescriptorSet[i];  // Descriptor set to update
            modelDescriptorWrite.dstBinding = 0;  // Binding 0 in the shader (model)
            modelDescriptorWrite.dstArrayElement = 0;
            modelDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            modelDescriptorWrite.descriptorCount = 1;  // One buffer
            modelDescriptorWrite.pBufferInfo = &modelBufferInfo;  // Buffer info

            VkDescriptorBufferInfo phongBufferInfo{};
            phongBufferInfo.buffer = mPhongBuffer[i];// the phong material buffer (dynamic)
            phongBufferInfo.offset = 0;
            phongBufferInfo.range = utils::AlignedSize(sizeof(PhongMaterialUniformBuffer), 1,
                vk::Instance::gInstance->GetPhysicalDevice());
            VkWriteDescriptorSet phongDescriptorWrite{};
            phongDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            phongDescriptorWrite.dstSet = mModelDescriptorSet[i];
            phongDescriptorWrite.dstBinding = 1;//phong is at binding 1
            phongDescriptorWrite.dstArrayElement = 0;
            phongDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            phongDescriptorWrite.descriptorCount = 1;
            phongDescriptorWrite.pBufferInfo = &phongBufferInfo;
            

            VkDescriptorImageInfo textureSamplerImageInfo{};
            textureSamplerImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            textureSamplerImageInfo.imageView = textureImageView;
            textureSamplerImageInfo.sampler = mPhongTextureSampler;
            VkWriteDescriptorSet textureSamplerDescriptorWrite{};
            textureSamplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            textureSamplerDescriptorWrite.dstSet = mPhongTextureSamplerDescriptorSet;
            textureSamplerDescriptorWrite.dstBinding = 0;
            textureSamplerDescriptorWrite.dstArrayElement = 0;
            textureSamplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            textureSamplerDescriptorWrite.descriptorCount = 1;
            textureSamplerDescriptorWrite.pImageInfo = &textureSamplerImageInfo;

            std::array<VkWriteDescriptorSet, 5> descriptorWrites = { 
                cameraDescriptorWrite,pointLightDescriptorWrite,
                modelDescriptorWrite, phongDescriptorWrite,
                textureSamplerDescriptorWrite };
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
            utils::CreateAlignedBuffer(sizeof(ModelUniformBuffer),
                MAX_NUMBER_OF_OBJS,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                mModelBuffer[i], mModelBufferMemory[i]);
        }
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            utils::CreateAlignedBuffer(sizeof(PhongMaterialUniformBuffer),
                MAX_NUMBER_OF_OBJS,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                mPhongBuffer[i], mPhongMemory[i]);
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
        ////CANT BIND IT HERE BECAUSE IT'S SHARED WITH POINT LIGHT UNIFORM
        ////bind camera descriptor set
        //vkCmdBindDescriptorSets(cmdBuffer, 
        //    VK_PIPELINE_BIND_POINT_GRAPHICS, 
        //    phong.mPipelineLayout, 
        //    0, //set 0 
        //    1,
        //    &phong.mCameraDescriptorSet[currentFrame], 0, nullptr);
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
        memcpy(data, &mModelData, sizeof(ModelUniformBuffer));
        vkUnmapMemory(device, phong.mModelBufferMemory[currentFrame]);
        //bind model descriptor set
        //vkCmdBindDescriptorSets(cmdBuffer,
        //    VK_PIPELINE_BIND_POINT_GRAPHICS,
        //    phong.mPipelineLayout,
        //    1, //set 0 
        //    1,
        //    &phong.mModelDescriptorSet[currentFrame],
        //    1, //number of dynamic offsets
        //    &offset); //dynamic offset


    }

    void PhongMaterialUniform::Set(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        assert(mModelId != UINT32_MAX);
        static VkPhysicalDevice physicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
        //get my pipeline
        const SolidPhongPipeline& phong = dynamic_cast<const SolidPhongPipeline&>(pipeline);
        //map model data and copy to the gpu, mind the offsets and aligned size
        const auto device = vk::Device::gDevice->GetDevice();
        void* data;
        VkDeviceSize alignedSize = utils::AlignedSize(sizeof(PhongMaterialUniformBuffer),
            1, physicalDevice);
        uint32_t offset = alignedSize * mModelId;
        vkMapMemory(device,
            phong.mPhongMemory[currentFrame],
            offset, //offset
            alignedSize, //size
            0, &data);
        memcpy(data, &this->mMaterial, sizeof(PhongMaterialUniformBuffer));
        vkUnmapMemory(device, phong.mPhongMemory[currentFrame]);

        ////bind phong descriptor set
        //vkCmdBindDescriptorSets(cmdBuffer,
        //    VK_PIPELINE_BIND_POINT_GRAPHICS,
        //    phong.mPipelineLayout,
        //    1, //set 0 
        //    1,
        //    &phong.mModelDescriptorSet[currentFrame],
        //    1, //number of dynamic offsets
        //    &offset); //dynamic offset
    }

    PhongMaterialUniform::PhongMaterialUniform(uint32_t modelId) :
        mModelId(modelId), mMaterial()
    {
    }

}
