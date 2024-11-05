#include "imgui_utils.h"
#include <array>
#include <vk/device.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <vk/instance.h>
constexpr int MIN_IMAGE_COUNT = 2;
typedef std::array<VkDescriptorPoolSize, 10> pool_sizes_t;
pool_sizes_t GetPoolSizes();
app::ImguiUtils::ImguiUtils(
	GLFWwindow* window, 
	uint32_t numberOfImagesInTheSwapChain,
	VkRenderPass renderPass):
	numberOfImages(numberOfImagesInTheSwapChain)
{
	VkDevice device = vk::Device::gDevice->GetDevice();
	////Create descriptor pool for imgui
	pool_sizes_t pool_sizes = GetPoolSizes();
	VkDescriptorPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * pool_sizes.size();
	pool_info.poolSizeCount = pool_sizes.size();
	pool_info.pPoolSizes = pool_sizes.data();
	vkCreateDescriptorPool(device, &pool_info, nullptr, &imguiPool);
	////Init imgui lib
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForVulkan(window, true);
	ImGui_ImplVulkan_InitInfo init_info{};
	init_info.Instance = vk::Instance::gInstance->GetInstance();
	init_info.PhysicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
	init_info.Device = device;
	init_info.Queue = vk::Device::gDevice->GetGraphicsQueue();
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = MIN_IMAGE_COUNT;
	init_info.ImageCount = numberOfImages;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Subpass = 0;
	init_info.UseDynamicRendering = false;
	init_info.RenderPass = renderPass;
	ImGui_ImplVulkan_Init(&init_info);
	ImGui_ImplVulkan_CreateFontsTexture();
}

app::ImguiUtils::~ImguiUtils()
{
	VkDevice device = vk::Device::gDevice->GetDevice();
	ImGui_ImplVulkan_DestroyFontsTexture();
	vkDestroyDescriptorPool(device, imguiPool, nullptr);
	ImGui_ImplVulkan_Shutdown();
}

void app::ImguiUtils::BeginImguiFrame()
{
	// Start ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame(); 
	ImGui::NewFrame();
}



pool_sizes_t GetPoolSizes() {
	std::array<VkDescriptorPoolSize, 10> pool_sizes;
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	pool_sizes[0].descriptorCount = 1000;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = 1000;
	pool_sizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	pool_sizes[2].descriptorCount = 1000;
	pool_sizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	pool_sizes[3].descriptorCount = 1000;
	pool_sizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	pool_sizes[4].descriptorCount = 1000;
	pool_sizes[5].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[5].descriptorCount = 1000;
	pool_sizes[6].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	pool_sizes[6].descriptorCount = 1000;
	pool_sizes[7].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	pool_sizes[7].descriptorCount = 1000;
	pool_sizes[8].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	pool_sizes[8].descriptorCount = 1000;
	pool_sizes[9].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	pool_sizes[9].descriptorCount = 1000;
	return pool_sizes;
}
