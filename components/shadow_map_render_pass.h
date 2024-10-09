#pragma once
#include "vk/render_pass.h"
#include "data_structures/ring_buffer.h"
#include <memory>
#include "vk/depth_buffer.h"

namespace components {
	class DepthBuffer;
	class ShadowMapRenderPass : public vk::RenderPass {
	public:
		ShadowMapRenderPass(uint32_t w, uint32_t h, uint32_t numberOfColorAttachments);
		~ShadowMapRenderPass();
	private:
		std::vector<vk::DepthBuffer*> mBuffers;
		void SetRenderPass();
		void SetFramebuffer();
		VkFormat mFormat;
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mNumberOfColorAttachments;
		std::vector<VkFramebuffer> mFramebuffers;
		// Inherited via RenderPass
		VkFramebuffer GetFramebuffer(uint32_t imageIndex) override;
	};
}