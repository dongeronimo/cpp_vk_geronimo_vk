#include "shadow_map_render_pass.h"
#include <vk/device.h>
#include <utils/vk_utils.h>
#include <vk/instance.h>
#include <vk/debug_utils.h>
#include <vk/depth_buffer.h>
#include <array>
#include <stdexcept>
#include <utils/concatenate.h>
#include <cassert>

components::PointLightsShadowMapRenderPass::PointLightsShadowMapRenderPass(std::string name, uint32_t w, uint32_t h, uint32_t numberOfColorAttachments)
	:vk::RenderPass(name, { 0.3f, 0.3f, 0.3f, 1.0f }),
	mWidth(w),
	mHeight(h),
	mNumberOfColorAttachments(numberOfColorAttachments)
{
	mFormat = utils::FindDepthFormat(vk::Instance::gInstance->GetPhysicalDevice());
	//Set the clear values
	mClearValues.resize(1);
	mClearValues[0] = { 1.0f, 0 };
	mExtent.width = w;
	mExtent.height = h;
	//create the render pass
	SetRenderPass();
	//create the frame buffer
	SetFramebuffer();
}

void components::PointLightsShadowMapRenderPass::EndRenderPass(VkCommandBuffer cmdBuffer)
{
	vk::RenderPass::EndRenderPass(cmdBuffer);
	///TODO shadow: transition image layout
	assert(mImageIndex != UINT32_MAX);
	auto depthBuffer = mBuffers.at(mImageIndex);

	// Define the depth image barrier
	VkImageMemoryBarrier depthImageBarrier = {};
	depthImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	depthImageBarrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//from depth stencil
	depthImageBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;//to sampleable-by-shader
	depthImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; //same queue family
	depthImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	depthImageBarrier.image = depthBuffer->GetImage();  // Your depth image handle
	depthImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthImageBarrier.subresourceRange.baseMipLevel = 0;
	depthImageBarrier.subresourceRange.levelCount = 1;
	depthImageBarrier.subresourceRange.baseArrayLayer = 0;
	depthImageBarrier.subresourceRange.layerCount = 1;

	// Set up the access masks
	depthImageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;  // Depth write in shadow pass
	depthImageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;                     // Shader read in next pass

	// Define pipeline stages
	VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	//// Issue the pipeline barrier at the end of the shadow pass
	//vkCmdPipelineBarrier(
	//	cmdBuffer,
	//	srcStageMask,            // Source stage (depth writing)
	//	dstStageMask,            // Destination stage (shader reading)
	//	0,                       // No additional flags
	//	0, nullptr,              // No memory barriers
	//	0, nullptr,              // No buffer barriers
	//	1, &depthImageBarrier    // Transition the depth image
	//);
	mImageIndex = UINT32_MAX;


}

void components::PointLightsShadowMapRenderPass::BeginRenderPass(VkCommandBuffer cmdBuffer, uint32_t imageIndex, uint32_t frameNumber)
{
	/*The depth buffer will be unuseable when i enter here for the second time because it's transitioned to shader sampler layout
	in the end of the render pass. So i have to revert it back to writable by the depth calculations here, before doing thing with it*/

	//if (firstRun) {
	//	firstRun = false; //the 1st time i run this function the image buffer will be in a correct layout so we can skip
	//	vk::RenderPass::BeginRenderPass(cmdBuffer, imageIndex, frameNumber);
	//	return;
	//}
	//assert(mImageIndex != UINT32_MAX);
	//auto depthBuffer = mBuffers.at(mImageIndex);

	//// Define the depth image barrier
	//VkImageMemoryBarrier depthImageBarrier = {};
	//depthImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//depthImageBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//from sampleable by shader
	//depthImageBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;//to depth stencil
	//depthImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; //same queue family
	//depthImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//depthImageBarrier.image = depthBuffer->GetImage();  // Your depth image handle
	//depthImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	//depthImageBarrier.subresourceRange.baseMipLevel = 0;
	//depthImageBarrier.subresourceRange.levelCount = 1;
	//depthImageBarrier.subresourceRange.baseArrayLayer = 0;
	//depthImageBarrier.subresourceRange.layerCount = 1;

	//// Set up the access masks
	//depthImageBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	//depthImageBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;                   

	//// Define pipeline stages
	//VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	//VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	//// Issue the pipeline barrier at the end of the shadow pass
	//vkCmdPipelineBarrier(
	//	cmdBuffer,
	//	srcStageMask,            // Source stage (depth writing)
	//	dstStageMask,            // Destination stage (shader reading)
	//	0,                       // No additional flags
	//	0, nullptr,              // No memory barriers
	//	0, nullptr,              // No buffer barriers
	//	1, &depthImageBarrier    // Transition the depth image
	//);
	mImageIndex = imageIndex;
	vk::RenderPass::BeginRenderPass(cmdBuffer, imageIndex, frameNumber);
}

void components::PointLightsShadowMapRenderPass::SetRenderPass()
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
	renderPassCreateInfo.dependencyCount = dependencies.size();
	renderPassCreateInfo.pDependencies = dependencies.data();
	vkCreateRenderPass(vk::Device::gDevice->GetDevice(), &renderPassCreateInfo, nullptr, &mRenderPass);
	SET_NAME(mRenderPass, VK_OBJECT_TYPE_RENDER_PASS, mName.c_str());
}

void components::PointLightsShadowMapRenderPass::SetFramebuffer()
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

VkFramebuffer components::PointLightsShadowMapRenderPass::GetFramebuffer(uint32_t imageIndex)
{
	return mFramebuffers[imageIndex];
}

components::PointLightsShadowMapRenderPass::~PointLightsShadowMapRenderPass()
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