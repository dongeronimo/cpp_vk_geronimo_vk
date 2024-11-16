#pragma once
#include "vk/render_pass.h"
#include "data_structures/ring_buffer.h"
#include <memory>
namespace vk {
	class SwapChain;
	class DepthBuffer;
}
namespace components {
	/// <summary>
	/// The render pass that'll actually show things on screen. It's framebuffer comes from
	/// the swapchain
	/// </summary>
	class MainRenderPass : public vk::RenderPass {
	public:
		/// <summary>
		/// Creates the swap chain.
		/// </summary>
		MainRenderPass();
		~MainRenderPass();
		//Set the clear color for the color buffer. Depth is fixed and will always clear with 1.
		void SetClearColor(std::array<float, 4> color);
		uint32_t GetNumberOfSwapChainColorAttachments()const {
			return mFramebuffers.size();
		}
		std::shared_ptr<vk::SwapChain> GetSwapChain()const { return mSwapChain; }
		void DestroyFramebuffers();
		void DestroyRenderPass();
		void DestroyDepthBuffers();
		void Recreate();
		std::vector<VkFramebuffer> GetFramebuffers()const {
			return mFramebuffers;
		}
	private:
		/// <summary>
		/// The swap chain.
		/// </summary>
		std::shared_ptr<vk::SwapChain> mSwapChain;
		std::vector<VkFramebuffer> mFramebuffers;
		// Inherited via RenderPass
		VkFramebuffer GetFramebuffer(uint32_t imageIndex) override;
		VkFormat mColorFormat;
		VkFormat mDepthFormat;
		std::shared_ptr<vk::DepthBuffer> mDepthBuffer;
		void RecreateRenderPass();
	};
}