#include "main_render_pass.h"
#include "vk\swap_chain.h"
#include "vk\device.h"
#include <cassert>
#include <stdexcept>
#include <utils/concatenate.h>
#include <vk/debug_utils.h>
#include "vk/depth_buffer.h"
namespace components
{
    void MainRenderPass::Recreate()
    {
        mSwapChain->Recreate();
        mColorFormat = mSwapChain->GetFormat();
        mDepthBuffer = std::make_shared<vk::DepthBuffer>(mSwapChain->GetExtent().width, mSwapChain->GetExtent().height);
        mDepthFormat = mDepthBuffer->GetFormat();
        RecreateRenderPass();
    }
    MainRenderPass::MainRenderPass()
        :vk::RenderPass("mainRenderPass", {1.0f, 0.3f, 0.6f, 1.0f})
    {
        mClearValues.resize(2);
        SetClearColor({ 0.3f,0,0.3f,1 });

        ///Create the swapchain
        mSwapChain = std::make_shared<vk::SwapChain>();
        mColorFormat = mSwapChain->GetFormat();
        //create the depth image
        mDepthBuffer = std::make_shared<vk::DepthBuffer>(mSwapChain->GetExtent().width, mSwapChain->GetExtent().height);
        mDepthFormat = mDepthBuffer->GetFormat();
        RecreateRenderPass();
    }
    MainRenderPass::~MainRenderPass()
    {
        for (auto& f : mFramebuffers) {
            vkDestroyFramebuffer(vk::Device::gDevice->GetDevice(), f, nullptr);
        }
        mSwapChain.reset();
    }
    void MainRenderPass::SetClearColor(std::array<float, 4> color)
    {
        mClearValues[0].color = { {color[0], color[1], color[2], color[3]}};
        mClearValues[1].depthStencil = { 1.0f, 0 };

    }
    void MainRenderPass::DestroyFramebuffers()
    {
        for (auto& f : mFramebuffers) {
            vkDestroyFramebuffer(vk::Device::gDevice->GetDevice(),
                f, nullptr);
        }
    }
    void MainRenderPass::DestroyRenderPass()
    {
        vkDestroyRenderPass(vk::Device::gDevice->GetDevice(),
            mRenderPass, nullptr);
    }
    void MainRenderPass::DestroyDepthBuffers()
    {
        mDepthBuffer.reset();
        assert(mDepthBuffer.use_count() == 0);
    }

    VkFramebuffer MainRenderPass::GetFramebuffer(uint32_t imageIndex)
    {
        return mFramebuffers[imageIndex];
    }
    void MainRenderPass::RecreateRenderPass()
    {
        ///create the render pass
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = mDepthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        //the image that'll hold the result
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = mColorFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//clear the values to a constant at the start
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //store the rendered result in memory for later uses
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//not using the stencil buffer
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//image presented in the swap chain
        //the reference to the image attachment that'll hold the result
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //the render subpass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        //the actual render pass, composed of image attachments and subpasses
        std::array<VkAttachmentDescription, 2> attachments = {
            colorAttachment,
            depthAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        //the render pass depends upon the previous render pass to run
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        //wait until the image is attached
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(vk::Device::gDevice->GetDevice(), &renderPassInfo, nullptr,
            &mRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
        SET_NAME(mRenderPass, VK_OBJECT_TYPE_RENDER_PASS, mName.c_str());

        //Create the framebuffer for color and depth, color coming from the swapchain
        mExtent = mSwapChain->GetExtent();
        const auto colorAttachments = mSwapChain->GetImageViews();

        mFramebuffers.resize(colorAttachments.size());
        for (size_t i = 0; i < colorAttachments.size(); i++) {
            VkImageView colorAttachmentIV = colorAttachments[i];
            std::vector<VkImageView> attachments{
                colorAttachmentIV, mDepthBuffer->GetImageView()
            };
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = mRenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = mSwapChain->GetExtent().width;
            framebufferInfo.height = mSwapChain->GetExtent().height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vk::Device::gDevice->GetDevice(),
                &framebufferInfo, nullptr,
                &mFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
            auto _name = Concatenate(mName, "Framebuffer ", i);
            SET_NAME(mFramebuffers[i], VK_OBJECT_TYPE_FRAMEBUFFER, _name.c_str());
        }
    }
}
