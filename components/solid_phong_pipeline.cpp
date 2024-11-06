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
#include "components/renderable.h"
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
        rasterizer.cullMode = VK_CULL_MODE_NONE;
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
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
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
    SolidPhongPipeline::SolidPhongPipeline(const vk::RenderPass& rp, const std::vector<VkImageView>& shadowMapImageViews,
        VkImageView texture, VkSampler sampler)
        :vk::Pipeline("SolidPhongPipeline", rp)
    {
        const auto device = vk::Device::gDevice->GetDevice();
        CreateDescriptorSetLayout();
 
        CreateDescriptorPool();
        CreateCameraBuffer();
        CreateModelBuffer();
        CreateDirectionalLightDataBuffer();
        CreateDescriptorSet(shadowMapImageViews, sampler, texture);
        CreatePipelineLayout();
        ////Load the shaders////
        mVertexShader = vk::LoadShaderModule(device, "phong.vert.spv");
        mFragmentShader = vk::LoadShaderModule(device, "phong.frag.spv");
        Recreate();
    }

    SolidPhongPipeline::~SolidPhongPipeline()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        vkDestroySampler(device, mShadowDepthSampler, nullptr);
        vkDestroyShaderModule(device, mVertexShader, nullptr);
        vkDestroyShaderModule(device, mFragmentShader, nullptr);

        for (auto& m : mCameraBuffer) {
            vkDestroyBuffer(device, m, nullptr);
        }
        for (auto& m : mCameraBufferMemory) {
            vkFreeMemory(device, m, nullptr);
        }
        //master: missing deletions
        for (auto& m : mModelBuffer) {
            vkDestroyBuffer(device, m, nullptr);
        }
        for (auto& m : mModelBufferMemory) {
            vkFreeMemory(device, m, nullptr);
        }
        for (auto& m : mDirectionalLightBuffer) {
            vkDestroyBuffer(device, m, nullptr);
        }
        for (auto& m : mDirectionalLightMemory) {
            vkFreeMemory(device, m, nullptr);
        }
        //vkDestroyDescriptorPool(device, mDescriptorPool, nullptr);
    }
    void SolidPhongPipeline::ActivateShadowMap(uint32_t framebufferImageNumber, VkCommandBuffer buffer)
    {
        vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 2, 1, &mShadowMapDescriptorSet[framebufferImageNumber], 0, nullptr);
    }
    void SolidPhongPipeline::Bind(VkCommandBuffer buffer, uint32_t currentFrame)
    {       
        Pipeline::Bind(buffer, currentFrame);
        ///Bind the sampler and the texture
        vkCmdBindDescriptorSets(buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mPipelineLayout,
            4, //set number 4, where we have the phong textures and it's sampler
            1,
            &mPhongTexturesDescriptorSet,
            0, //number of dynamic offsets
            nullptr); //dynamic offset
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

    void SolidPhongPipeline::CreateDepthSampler()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;  // Magnification filter
        samplerInfo.minFilter = VK_FILTER_LINEAR;  // Minification filter
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;  // Clamps coordinates outside the image to the edge
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_FALSE;  // Anisotropy not needed for depth sampling
        samplerInfo.maxAnisotropy = 1.0f;  // Only relevant if anisotropy is enabled
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;  // Border color, used with clamp-to-border mode (not in this case)
        samplerInfo.unnormalizedCoordinates = VK_FALSE;  // Use normalized coordinates (range [0, 1])
        samplerInfo.compareEnable = VK_TRUE;  // Enable comparison, important for shadow mapping
        samplerInfo.compareOp = VK_COMPARE_OP_LESS;  // Comparison function for depth comparison (e.g., shadow mapping)
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;  // Mipmapping, though not relevant for a single depth image
        samplerInfo.minLod = 0.0f;  // Min level of detail
        samplerInfo.maxLod = 0.0f;  // Max level of detail

        VkResult result = vkCreateSampler(device, &samplerInfo, nullptr, &mShadowDepthSampler);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create depth sampler!");
        }
        auto _n = Concatenate(mName, "DepthSampler");
        SET_NAME(mShadowDepthSampler, VK_OBJECT_TYPE_SAMPLER, _n.c_str());
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

        VkDescriptorSetLayoutBinding modelBinding;
        modelBinding.binding = 0; // Matches binding 0 in the shader
        modelBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        modelBinding.descriptorCount = 1;
        modelBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        modelBinding.pImmutableSamplers = nullptr; // Not used
        VkDescriptorSetLayoutBinding phongPropertiesBindings;
        phongPropertiesBindings.binding = 1; // Matches binding 1 in the shader
        phongPropertiesBindings.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        phongPropertiesBindings.descriptorCount = 1;
        phongPropertiesBindings.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        phongPropertiesBindings.pImmutableSamplers = nullptr; // Not used
        VkDescriptorSetLayoutCreateInfo modelLayoutInfo{};
        std::array< VkDescriptorSetLayoutBinding, 2> modelBindings{ modelBinding, phongPropertiesBindings };
        modelLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        modelLayoutInfo.bindingCount = modelBindings.size(); // Only one binding, for the Camera uniform buffer
        modelLayoutInfo.pBindings = modelBindings.data();
        VkDescriptorSetLayout modelDescriptorSetLayout;
        if (vkCreateDescriptorSetLayout(device, &modelLayoutInfo, nullptr, &modelDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }
        auto n2 = Concatenate(mName, "ModelDescriptorSetLayout");
        SET_NAME(modelDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n2.c_str());
        ////////////////////////////////////////////////////////////////////////
        VkDescriptorSetLayoutBinding shadowMapBindings{};
        shadowMapBindings.binding = 0;
        shadowMapBindings.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        shadowMapBindings.descriptorCount = 1;
        shadowMapBindings.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        shadowMapBindings.pImmutableSamplers = nullptr;  // No immutable samplers

        VkDescriptorSetLayoutCreateInfo shadowMapLayoutInfo{};
        shadowMapLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        shadowMapLayoutInfo.bindingCount = 1;
        shadowMapLayoutInfo.pBindings = &shadowMapBindings;

        VkDescriptorSetLayout shadowMapDescriptorSetLayout = VK_NULL_HANDLE;
        if (vkCreateDescriptorSetLayout(device, &shadowMapLayoutInfo, nullptr, &shadowMapDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }
        auto n3 = Concatenate(mName, "ShadowMapDescriptorSetLayout");
        SET_NAME(shadowMapDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n3.c_str());
        //////////////////////////////////////////////////////////////////////////////////
        VkDescriptorSetLayoutBinding directionalLightBinding;
        directionalLightBinding.binding = 0;
        directionalLightBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        directionalLightBinding.descriptorCount = 1;
        directionalLightBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        VkDescriptorSetLayoutCreateInfo directionalLightLayoutInfo{};
        directionalLightLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        directionalLightLayoutInfo.bindingCount = 1;
        directionalLightLayoutInfo.pBindings = &directionalLightBinding;
        VkDescriptorSetLayout directionalLightLayout = VK_NULL_HANDLE;
        vkCreateDescriptorSetLayout(device, &directionalLightLayoutInfo, nullptr, &directionalLightLayout);
        auto n4 = Concatenate(mName, "DirectionalLightDescriptorSetLayout");
        SET_NAME(directionalLightLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n4.c_str());
        ///////////////////////////////////////////////////////////////////////////////////
        VkDescriptorSetLayoutBinding phongSamplerBinding;
        phongSamplerBinding.binding = 0;
        phongSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        phongSamplerBinding.descriptorCount = 1;
        phongSamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        phongSamplerBinding.pImmutableSamplers = nullptr;
        VkDescriptorSetLayoutBinding phongDiffuseTextureBinding;
        phongDiffuseTextureBinding.binding = 1;
        phongDiffuseTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        phongDiffuseTextureBinding.descriptorCount = 1;
        phongDiffuseTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        phongDiffuseTextureBinding.pImmutableSamplers = nullptr;
        VkDescriptorSetLayoutBinding phongSpecularTextureBinding;
        phongSpecularTextureBinding.binding = 2;
        phongSpecularTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        phongSpecularTextureBinding.descriptorCount = 1;
        phongSpecularTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        phongSpecularTextureBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo phongTextureLayoutInfo{};
        phongTextureLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        std::array< VkDescriptorSetLayoutBinding, 3> phongBindings{ phongSamplerBinding, phongDiffuseTextureBinding, phongSpecularTextureBinding };
        phongTextureLayoutInfo.bindingCount = phongBindings.size();
        phongTextureLayoutInfo.pBindings = phongBindings.data();
        phongTextureLayoutInfo.flags = 0;
        VkDescriptorSetLayout phongLayout = VK_NULL_HANDLE;
        VkResult result = vkCreateDescriptorSetLayout(device, &phongTextureLayoutInfo,
            nullptr, &phongLayout);
        auto n5 = Concatenate(mName, "phongTexturesDescriptorSetLayout");
        SET_NAME(phongLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, n5.c_str());

        //Order matter!
        mDescriptorSetLayouts.push_back(modelDescriptorSetLayout); //set 0 - model matrix
        mDescriptorSetLayouts.push_back(cameraDescriptorSetLayout); //set 1 - camera (view and proj)
        mDescriptorSetLayouts.push_back(shadowMapDescriptorSetLayout); //set 2 - directional light shadow map
        mDescriptorSetLayouts.push_back(directionalLightLayout);//set 3 - directional light properties
        mDescriptorSetLayouts.push_back(phongLayout); //set 4 - phong sampler + diffuse texture
    }

    void SolidPhongPipeline::CreateDescriptorPool()
    {
        const auto device = vk::Device::gDevice->GetDevice();
        std::array<VkDescriptorPoolSize, 7> poolSizes;
        // Camera - uniform buffer, one per frame
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;

        // Model and phong properties- dynamic buffer, one per frame
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT; 
        poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[2].descriptorCount = MAX_FRAMES_IN_FLIGHT;
        // Shadow map - combined image sampler, one per frame
        poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[3].descriptorCount = 20 *MAX_FRAMES_IN_FLIGHT;  
        // Directional light data
        poolSizes[4].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[4].descriptorCount = MAX_FRAMES_IN_FLIGHT;
        // phong material textures
        poolSizes[5].type = VK_DESCRIPTOR_TYPE_SAMPLER;
        poolSizes[5].descriptorCount = 1;
        poolSizes[6].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        poolSizes[6].descriptorCount = 2;
        /////Create the descriptor pool/////
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 2+ //One for the model
            1+ //one for the camera
            1+ //one for the directional light
            1 + 1 + //for phong textures
            5 * //a lot for the directional light shadow map sampler 
            MAX_FRAMES_IN_FLIGHT; 
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

    void SolidPhongPipeline::CreateDescriptorSet(const std::vector<VkImageView>& shadowMapImageViews,
        VkSampler phongSampler, VkImageView phongDiffuseImageView)
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
        //shadow map descriptor set - their size is based on the number of images in the framebuffer, not in max_frames_in_flight!
        std::vector<VkDescriptorSetLayout> shadowMapLayouts(shadowMapImageViews.size(),
            mDescriptorSetLayouts[2]);
        mShadowMapDescriptorSet.resize(shadowMapImageViews.size());
        VkDescriptorSetAllocateInfo shadowMapAllocInfo{};
        shadowMapAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        shadowMapAllocInfo.descriptorPool = mDescriptorPool;
        shadowMapAllocInfo.descriptorSetCount = static_cast<uint32_t>(shadowMapImageViews.size());
        shadowMapAllocInfo.pSetLayouts = shadowMapLayouts.data();
        if (vkAllocateDescriptorSets(device, &shadowMapAllocInfo, 
            mShadowMapDescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate shadow map descriptor sets!");
        }

        ///directional light data descriptor set
        std::vector<VkDescriptorSetLayout> directionalLightDataLayouts(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayouts[3]);
        VkDescriptorSetAllocateInfo allocInfoDirectionalLight{};
        allocInfoDirectionalLight.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfoDirectionalLight.descriptorPool = mDescriptorPool;  // Pool created earlier
        allocInfoDirectionalLight.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfoDirectionalLight.pSetLayouts = directionalLightDataLayouts.data();  // Layout for directional light data
        if (vkAllocateDescriptorSets(device, &allocInfoCamera, mDirectionalLightDescriptorSet.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate camera descriptor sets!");
        }

        //phong textures descriptor set
        //create the descriptor set
        std::vector<VkDescriptorSetLayout> phongTexturesLayout(1, 
            mDescriptorSetLayouts[4]);
        VkDescriptorSetAllocateInfo phongTexturesAllocInfo{};
        phongTexturesAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        phongTexturesAllocInfo.descriptorPool = mDescriptorPool;
        phongTexturesAllocInfo.descriptorSetCount = phongTexturesLayout.size();
        phongTexturesAllocInfo.pSetLayouts = phongTexturesLayout.data();
        if (vkAllocateDescriptorSets(device, &phongTexturesAllocInfo,
            &mPhongTexturesDescriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate phong textures descriptor sets!");
        }
        //update it
        // Descriptor write for the sampler (binding 0)
        VkDescriptorImageInfo phongSamplerInfo{};
        phongSamplerInfo.sampler = phongSampler;
        VkWriteDescriptorSet phongSamplerWrite{};
        phongSamplerWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        phongSamplerWrite.dstSet = mPhongTexturesDescriptorSet;
        phongSamplerWrite.dstBinding = 0; // Binding index for sampler
        phongSamplerWrite.dstArrayElement = 0;
        phongSamplerWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        phongSamplerWrite.descriptorCount = 1;
        phongSamplerWrite.pImageInfo = &phongSamplerInfo;
        // Descriptor write for the diffuse texture (binding 1)
        VkDescriptorImageInfo phongDiffuseTextureInfo{};
        phongDiffuseTextureInfo.imageView = phongDiffuseImageView;
        phongDiffuseTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        VkWriteDescriptorSet phongDiffuseTextureWrite{};
        phongDiffuseTextureWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        phongDiffuseTextureWrite.dstSet = mPhongTexturesDescriptorSet;
        phongDiffuseTextureWrite.dstBinding = 1; // Binding index for texture
        phongDiffuseTextureWrite.dstArrayElement = 0;
        phongDiffuseTextureWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        phongDiffuseTextureWrite.descriptorCount = 1;
        phongDiffuseTextureWrite.pImageInfo = &phongDiffuseTextureInfo;
        // Descriptor write for the specular texture (binding 2)
        VkDescriptorImageInfo phongSpecularTextureInfo{};
        phongSpecularTextureInfo.imageView = phongDiffuseImageView;
        phongSpecularTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        VkWriteDescriptorSet phongSpecularTextureWrite{};
        phongSpecularTextureWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        phongSpecularTextureWrite.dstSet = mPhongTexturesDescriptorSet;
        phongSpecularTextureWrite.dstBinding = 2; // Binding index for texture
        phongSpecularTextureWrite.dstArrayElement = 0;
        phongSpecularTextureWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        phongSpecularTextureWrite.descriptorCount = 1;
        phongSpecularTextureWrite.pImageInfo = &phongSpecularTextureInfo;

        // Update the descriptor set with both writes
        std::array<VkWriteDescriptorSet, 3> phongDescriptorWrites = { 
            phongSamplerWrite, phongDiffuseTextureWrite, phongSpecularTextureWrite
        };
        vkUpdateDescriptorSets(device, 
            static_cast<uint32_t>(phongDescriptorWrites.size()),
            phongDescriptorWrites.data(), 0, nullptr);

        CreateDepthSampler();
        for (size_t i = 0; i < shadowMapImageViews.size(); i++)
        {
            VkDescriptorImageInfo shadowMapImageInfo{};
            shadowMapImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;  // Layout for reading depth
            shadowMapImageInfo.imageView = shadowMapImageViews[i];  // The depth image view for the shadow map (one per frame)
            shadowMapImageInfo.sampler = mShadowDepthSampler;  // The sampler you created earlier
            VkWriteDescriptorSet shadowMapWrite{};
            shadowMapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            shadowMapWrite.dstSet = mShadowMapDescriptorSet.at(i);
            shadowMapWrite.dstBinding = 0;
            shadowMapWrite.dstArrayElement = 0;
            shadowMapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            shadowMapWrite.descriptorCount = 1;
            shadowMapWrite.pImageInfo = &shadowMapImageInfo;
            std::array<VkWriteDescriptorSet,1> descriptorWrites = { shadowMapWrite };
            // Perform the update
            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

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
            modelBufferInfo.range = utils::AlignedSize(sizeof(ModelUniformBuffer), 1, vk::Instance::gInstance->GetPhysicalDevice());  // Size of model data
            VkWriteDescriptorSet modelDescriptorWrite{};
            modelDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            modelDescriptorWrite.dstSet = mModelDescriptorSet[i];  // Descriptor set to update
            modelDescriptorWrite.dstBinding = 0;  // Binding 0 in the shader (model)
            modelDescriptorWrite.dstArrayElement = 0;
            modelDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            modelDescriptorWrite.descriptorCount = 1;  // One buffer
            modelDescriptorWrite.pBufferInfo = &modelBufferInfo;  // Buffer info

            VkDescriptorBufferInfo phongPropertiesBufferInfo{};
            phongPropertiesBufferInfo.buffer = mPhongPropertiesBuffer[i];
            phongPropertiesBufferInfo.offset = 0;  
            phongPropertiesBufferInfo.range = utils::AlignedSize(sizeof(PhongProperties), 1, vk::Instance::gInstance->GetPhysicalDevice());  
            VkWriteDescriptorSet phongPropertiesDescriptorWrite{};
            phongPropertiesDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            phongPropertiesDescriptorWrite.dstSet = mModelDescriptorSet[i];  // Descriptor set to update
            phongPropertiesDescriptorWrite.dstBinding = 1;  
            phongPropertiesDescriptorWrite.dstArrayElement = 0;
            phongPropertiesDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            phongPropertiesDescriptorWrite.descriptorCount = 1;  // One buffer
            phongPropertiesDescriptorWrite.pBufferInfo = &phongPropertiesBufferInfo;  // Buffer info



            VkDescriptorBufferInfo directionalLightBufferInfo{};
            directionalLightBufferInfo.buffer = mDirectionalLightBuffer[i];  
            directionalLightBufferInfo.offset = 0;  // Adjust offset per object/model
            directionalLightBufferInfo.range =  sizeof(DirectionalLightPropertiesUniformBuffer); 
            VkWriteDescriptorSet directionalLightDescriptorWrite{};
            directionalLightDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            directionalLightDescriptorWrite.dstSet = mDirectionalLightDescriptorSet[i];  // Descriptor set to update
            directionalLightDescriptorWrite.dstBinding = 0;  // Binding 0 in the shader (model)
            directionalLightDescriptorWrite.dstArrayElement = 0;
            directionalLightDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            directionalLightDescriptorWrite.descriptorCount = 1;  // One buffer
            directionalLightDescriptorWrite.pBufferInfo = &directionalLightBufferInfo;  // Buffer info


            std::array<VkWriteDescriptorSet, 4> descriptorWrites = { cameraDescriptorWrite, modelDescriptorWrite,
                phongPropertiesDescriptorWrite,directionalLightDescriptorWrite };
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

    void SolidPhongPipeline::CreateDirectionalLightDataBuffer()
    {
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            utils::CreateBuffer(sizeof(DirectionalLightPropertiesUniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                mDirectionalLightBuffer[i], mDirectionalLightMemory[i]);
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
            utils::CreateAlignedBuffer(sizeof(PhongProperties),
                MAX_NUMBER_OF_OBJS,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                mPhongPropertiesBuffer[i], mPhongPropertiesMemory[i]);
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
    void PhongPropertiesUniform::SetUniform(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        //get my pipeline
        if (pipeline.mHash == utils::Hash("SolidPhongPipeline")) {
            const SolidPhongPipeline& phong = dynamic_cast<const SolidPhongPipeline&>(pipeline);
            assert(mModelId != UINT32_MAX);
            static VkPhysicalDevice physicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
            static auto device = vk::Device::gDevice->GetDevice();
            void* data;
            VkDeviceSize alignedSize = utils::AlignedSize(sizeof(PhongProperties),
                1, physicalDevice);
            uint32_t offset = alignedSize * mModelId;
            vkMapMemory(device,
                phong.mPhongPropertiesMemory[currentFrame],
                offset, //offset
                alignedSize, //size
                0, &data);
            memcpy(data, &mPhongData, sizeof(PhongProperties));
            vkUnmapMemory(device, phong.mPhongPropertiesMemory[currentFrame]);
        }
    }
    void SolidPhongPipeline::Draw(components::Renderable& r, VkCommandBuffer cmdBuffer, uint32_t currentFrame)
    {
        static VkPhysicalDevice physicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
        VkDeviceSize modelAlignedSize = utils::AlignedSize(sizeof(ModelUniformBuffer),
            1, physicalDevice);
        uint32_t modelOffset = modelAlignedSize * r.GetModelId();
        VkDeviceSize phongPropsAlignedSize = utils::AlignedSize(sizeof(PhongProperties),
            1, physicalDevice);
        uint32_t phongPropsOffset = phongPropsAlignedSize * r.GetModelId();
        std::array<uint32_t, 2> offsets{ modelOffset, phongPropsOffset };
        vkCmdBindDescriptorSets(cmdBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mPipelineLayout,
            0, //set 0 
            1,
            &mModelDescriptorSet[currentFrame],
            offsets.size(), //number of dynamic offsets
            offsets.data()); //dynamic offset
        Pipeline::Draw(r, cmdBuffer, currentFrame);
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
            memcpy(data, &mModelData, sizeof(ModelUniformBuffer));
            vkUnmapMemory(device, phong.mModelBufferMemory[currentFrame]);
            //bind model descriptor set
            //vkCmdBindDescriptorSets(cmdBuffer,
            //    VK_PIPELINE_BIND_POINT_GRAPHICS,
            //    phong.mPipelineLayout,
            //    0, //set 0 
            //    1,
            //    &phong.mModelDescriptorSet[currentFrame],
            //    1, //number of dynamic offsets
            //    &offset); //dynamic offset
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
            memcpy(data, &mModelData, sizeof(ModelUniformBuffer));
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

    void DirectionalLightUniform::SetUniform(uint32_t currentFrame, const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        //get my pipeline
        const SolidPhongPipeline& phong = dynamic_cast<const SolidPhongPipeline&>(pipeline);
        //map camera data and copy to the gpu
        const auto device = vk::Device::gDevice->GetDevice();
        void* data;
        vkMapMemory(device,
            phong.mDirectionalLightMemory[currentFrame],
            0, sizeof(DirectionalLightPropertiesUniformBuffer), 0, &data);
        memcpy(data, &mLightData, sizeof(DirectionalLightPropertiesUniformBuffer));
        vkUnmapMemory(device, phong.mDirectionalLightMemory[currentFrame]);
        //bind camera descriptor set
        vkCmdBindDescriptorSets(cmdBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            phong.mPipelineLayout,
            3,
            1,
            &phong.mDirectionalLightDescriptorSet[currentFrame], 0, nullptr);
    }

    
}

