#pragma once
#include <string>
#include "utils\hash.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "data_structures/ring_buffer.h"
namespace vk
{
	/// <summary>
	/// A render pass. The render pass holds vulkan's render passes and framebuffers.
	/// This class, by itself, do little. The derived classes have to get their framebuffers, like the swap chain.
	/// 
	/// Usage:
	/// - You have to create derived classes from this base class. It is those derived class that create the vkRenderPass and the 
	/// framebuffers.
	/// - To use the render pass you call BeginRenderPass. In this class this method only begins the render pass with clear values.
	/// - To finish the render pass you call EndRenderPass. In this class this method only ends the render pass.
	/// 
	/// If you want to do more when the render pass begin or ends, like image and memory layout transition, you create your own BeginRenderPass
	/// or EndRenderPass and call the base class' methods.
	/// </summary>
	class RenderPass
	{
	public:
		/// <summary>
		/// Creates the render pass. It is expected, by the rest of the class, that by the end of this method
		/// mRenderPass is initialized. Derived classes must make sure of that.
		/// </summary>
		/// <param name="name"></param>
		RenderPass(const std::string& name, const std::array<float, 4>& markColor);
		virtual ~RenderPass();
		/// <summary>
		/// Begin the render pass doing a clear based on mClearValues
		/// </summary>
		/// <param name="cmdBuffer"></param>
		virtual void BeginRenderPass(VkCommandBuffer cmdBuffer, uint32_t imageIndex, uint32_t frameNumber);
		/// <summary>
		/// End the render pass
		/// </summary>
		/// <param name="cmdBuffer"></param>
		virtual void EndRenderPass(VkCommandBuffer cmdBuffer);
		/// <summary>
		/// Render pass name, mostly used for debugging. The vkRenderPass will have this name
		/// </summary>
		const std::string mName;
		/// <summary>
		/// The hash of the render pass.
		/// </summary>
		const hash_t mHash;
		/// <summary>
		/// THe color in renderDoc
		/// </summary>
		const std::array<float, 4> mDebugMarkColor;

		VkExtent2D GetExtent()const {
			return mExtent;
		}
		VkRenderPass GetRenderPass()const {
			return mRenderPass;
		}
	protected:
		/// <summary>
		/// The children know which framebuffer is to use.
		/// </summary>
		/// <returns></returns>
		virtual VkFramebuffer GetFramebuffer(uint32_t imageIndex) = 0;
		/// <summary>
		/// Remember to set the clear values in the children.
		/// </summary>
		std::vector<VkClearValue> mClearValues;
		/// <summary>
		/// Remeber to set the extent in the children.
		/// </summary>
		VkExtent2D mExtent = {};
		VkRenderPass mRenderPass = VK_NULL_HANDLE;
		bool mDebugInUse = false;
	};
}