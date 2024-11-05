#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_vulkan.h>
namespace app {
	class ImguiUtils {
	public:
		ImguiUtils(GLFWwindow* window, 
			uint32_t numberOfImagesInTheSwapChain,
			VkRenderPass renderPass);
		~ImguiUtils();
		void BeginImguiFrame();
	private:
		VkDescriptorPool imguiPool = VK_NULL_HANDLE;
		uint32_t numberOfImages;
	};
}