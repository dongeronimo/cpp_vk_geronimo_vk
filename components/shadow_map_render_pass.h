#pragma once
#include "vk/render_pass.h"
#include "data_structures/ring_buffer.h"
#include <memory>
#include "vk/depth_buffer.h"
#include <algorithm>
namespace components {
	class DepthBuffer;
	class PointLightsShadowMapRenderPass : public vk::RenderPass {
	public:
		PointLightsShadowMapRenderPass(std::string name,uint32_t w, uint32_t h, uint32_t numberOfColorAttachments);
		void EndRenderPass(VkCommandBuffer cmdBuffer) override;
		void BeginRenderPass(VkCommandBuffer cmdBuffer, uint32_t imageIndex, uint32_t frameNumber)override;
		std::vector<VkImageView> GetShadowBufferImageViews() {
			std::vector<VkImageView> r(mBuffers.size());
			std::transform(mBuffers.begin(), mBuffers.end(), r.begin(), [](vk::DepthBuffer* val) {
				return val->GetImageView();
				});
			return r;
		}
		~PointLightsShadowMapRenderPass();
	private:
		bool firstRun = true;
		std::vector<vk::DepthBuffer*> mBuffers;
		void SetRenderPass();
		void SetFramebuffer();
		VkFormat mFormat;
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mNumberOfColorAttachments;
		std::vector<VkFramebuffer> mFramebuffers;
		// Inherited via RenderPass
		uint32_t mImageIndex;
		VkFramebuffer GetFramebuffer(uint32_t imageIndex) override;
	};
}