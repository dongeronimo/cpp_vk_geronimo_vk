#include "window.h"
#include "vk\instance.h"
#include "vk\device.h"
#include <vk/debug_utils.h>
#include "components/main_render_pass.h"
#include "components/shadow_map_render_pass.h"
#include "components/shadow_map_pipeline.h"
#include "components/solid_phong_pipeline.h"
#include "vk/frame.h"
#include "vk/synchronization_service.h"
#include "io/mesh-load.h"
#include "components/mesh.h"
#include "components/camera.h"
#include "components/renderable.h"
#include "vk/swap_chain.h"
int main(int argc, char** argv)
{
	app::Window window(SCREEN_WIDTH, SCREEN_HEIGH);
	
	vk::Instance instance(window.GetWindow());
	instance.ChoosePhysicalDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, vk::YES);
	vk::Device device(instance.GetPhysicalDevice(), instance.GetInstance(), instance.GetSurface(), vk::GetValidationLayerNames());
	//////////////create render passes//////////////
	components::MainRenderPass mainRenderPass;
	components::ShadowMapRenderPass shadowMapRenderPass(512, 512, mainRenderPass.GetNumberOfSwapChainColorAttachments());
	//create pipelines
	components::SolidPhongPipeline* phongPipeline = new components::SolidPhongPipeline(mainRenderPass);
	//components::DirectionalLightShadowMapPipeline* directionaLightShadowMapPipeline = new components::DirectionalLightShadowMapPipeline();
	//create synchronization objects
	vk::SyncronizationService syncService;
	///////////////load meshes
	auto boxMeshData = io::LoadMeshes("box.glb");
	components::Mesh* boxMesh = new components::Mesh(boxMeshData[0]);//there can be many meshes per file, i know that in this file there's only one.
	size_t currentFrameId = 0;
	//////////////Create the camera
	components::Camera* camera = new components::Camera("mainCamera");
	camera->mFOV = glm::radians(60.0f);
	camera->mRatio = (float)mainRenderPass.GetExtent().width / (float)mainRenderPass.GetExtent().height;
	camera->mZNear = 0.1f;
	camera->mZFar = 100.0f;
	camera->mPosition = { 3, 7,11};
	camera->LookTo({ 0,0,0 });
	/////////////Create the game objects
	components::Renderable* myBox = new components::Renderable("MyBox", *boxMesh);
	////////////Create the command buffer
	ring_buffer_t<VkCommandBuffer> commandBuffers = device.CreateCommandBuffers("mainCommandBuffer");
	////////////On Resize
	std::function<void()> OnResize = [&device, &mainRenderPass, &commandBuffers, &phongPipeline, &camera, &window]() {
		int width = 0, height = 0;
		glfwGetFramebufferSize(window.GetWindow(), &width, &height);
		while (width == 0 || height == 0) {
			//it's minimized, lets wait for glfw events.
			glfwGetFramebufferSize(window.GetWindow(), &width, &height);
			glfwWaitEvents();
		}
		//wait for the device to stop doing all tasks
		vkDeviceWaitIdle(device.GetDevice());
		//destroy the structures affected by resize
		mainRenderPass.DestroyFramebuffers();
		vkFreeCommandBuffers(device.GetDevice(),
			device.GetCommandPool(), commandBuffers.size(), commandBuffers.data());
		phongPipeline->DestroyPipeline();
		mainRenderPass.DestroyRenderPass();
		mainRenderPass.GetSwapChain()->DestroyImageViews();
		mainRenderPass.GetSwapChain()->DestroySwapChain();
		//recreate them
		mainRenderPass.Recreate();
		phongPipeline->Recreate();
		camera->mRatio = (float)mainRenderPass.GetExtent().width / (float)mainRenderPass.GetExtent().height;
		commandBuffers = device.CreateCommandBuffers("mainCommandBuffer");
		};
	////////////OnRender
	window.OnRender = [&currentFrameId, &commandBuffers, &shadowMapRenderPass, &mainRenderPass, 
		&phongPipeline, &syncService, &camera, &myBox, &OnResize]
	(app::Window* wnd) {
		//TODO vulkan: do the rendering loop
		//begin the frame
		vk::Frame frame(commandBuffers,currentFrameId, syncService, *mainRenderPass.GetSwapChain());
		frame.OnResize = OnResize;
		frame.BeginFrame();
		//TODO Shadow: activate the render passes
		//shadowMapRenderPass.BeginRenderPass(frame.CommandBuffer(), frame.ImageIndex(), currentFrameId);
		////TODO Shadow: activate pipelines that use the render pass
		//directionaLightShadowMapPipeline->Bind();
		////TODO Shadow: draw meshes
		//TODO Shadow: end the render pass
		//shadowMapRenderPass.EndRenderPass(frame.CommandBuffer());
		mainRenderPass.BeginRenderPass(frame.CommandBuffer(), frame.ImageIndex(), currentFrameId);
		//activate pipelines that use the render pass
		phongPipeline->Bind(frame.CommandBuffer(), currentFrameId);
		//Apply camera
		camera->Set(currentFrameId, *phongPipeline, frame.CommandBuffer());
		//TODO vulkan: draw meshes
		phongPipeline->Draw(*myBox, frame.CommandBuffer());
		//end the render pass
		mainRenderPass.EndRenderPass(frame.CommandBuffer());
		//end the frame
		frame.EndFrame();
		currentFrameId = (currentFrameId + 1)%MAX_FRAMES_IN_FLIGHT;
	};
	window.OnResize = OnResize;
	window.MainLoop();
	//beginning shutdown
	syncService.WaitDeviceIdle();
	delete myBox;
	delete boxMesh;
	delete camera;
	delete phongPipeline;
	return 0;
}