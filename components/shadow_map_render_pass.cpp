#include "shadow_map_render_pass.h"
#include <vk/device.h>
#include <utils/vk_utils.h>
#include <vk/instance.h>
#include <vk/debug_utils.h>
#include <vk/depth_buffer.h>
#include <array>
#include <stdexcept>
#include <utils/concatenate.h>

components::ShadowMapRenderPass::ShadowMapRenderPass(uint32_t w, uint32_t h, uint32_t numberOfColorAttachments)
    :vk::RenderPass("ShadowMapRenderPass", {0.3f, 0.3f, 0.3f, 1.0f}), mWidth(w), mHeight(h), mNumberOfColorAttachments(numberOfColorAttachments)
{
	mFormat = utils::FindDepthFormat(vk::Instance::gInstance->GetPhysicalDevice());
    //Set the clear values
	mClearValues.resize(1);
	mClearValues[0] = { 1.0f, 0 };
	//create the render pass
	SetRenderPass();
    //create the frame buffer
	SetFramebuffer();
}

void components::ShadowMapRenderPass::SetRenderPass()
{
	VkAttachmentDescription attachmentDescription{};
	attachmentDescription.format = mFormat;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear depth at beginning of the render pass
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// We will read from depth, so it's important to store the depth attachment results
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					// We don't care about initial layout of the attachment
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;// Attachment will be transitioned to shader read at render pass end

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 0;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			// Attachment will be used as depth/stencil during render pass

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 0;													// No color attachments
	subpass.pDepthStencilAttachment = &depthReference;									// Reference to our depth attachment

	// Use subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &attachmentDescription;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassCreateInfo.pDependencies = dependencies.data();
	vkCreateRenderPass(vk::Device::gDevice->GetDevice(), &renderPassCreateInfo, nullptr, &mRenderPass);
	SET_NAME(mRenderPass, VK_OBJECT_TYPE_RENDER_PASS, mName.c_str());
}

void components::ShadowMapRenderPass::SetFramebuffer()
{
	//create the images
	mBuffers.resize(mNumberOfColorAttachments);
	for (uint32_t i = 0; i < mNumberOfColorAttachments; i++) {
		mBuffers[i] = new  vk::DepthBuffer(mWidth, mHeight);
		auto niv = Concatenate(mName, "ImageView", i);
		auto ni = Concatenate(mName, "Image", i);
		SET_NAME(mBuffers[i]->GetImageView(), VK_OBJECT_TYPE_IMAGE_VIEW, niv.c_str());
		SET_NAME(mBuffers[i]->GetImage(), VK_OBJECT_TYPE_IMAGE, ni.c_str());
	}
	mFramebuffers.resize(mNumberOfColorAttachments);
	//create the framebuffer
	for (size_t i = 0; i < mBuffers.size(); i++) {
		auto imageView = mBuffers[i]->GetImageView();
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = mRenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &imageView;
		framebufferInfo.width = mWidth;
		framebufferInfo.height = mHeight;
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

VkFramebuffer components::ShadowMapRenderPass::GetFramebuffer(uint32_t imageIndex)
{
	return mFramebuffers[imageIndex];
}

components::ShadowMapRenderPass::~ShadowMapRenderPass()
{
	const auto dev = vk::Device::gDevice->GetDevice();
	for (auto& f : mFramebuffers) {
		vkDestroyFramebuffer(dev, f, nullptr);
	}
	for (auto& i : mBuffers) {
		delete i;
	}
	mBuffers.clear();
}
