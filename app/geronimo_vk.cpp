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
#include "io/image-load.h"
#include <map>
#include "components/image_table.h"
#include <algorithm>

components::GpuTextureManager* gGPUTextureManager = nullptr;
std::vector<components::Renderable*> visibleObjects;
int main(int argc, char** argv)
{
	app::Window window(SCREEN_WIDTH, SCREEN_HEIGH);
	
	vk::Instance instance(window.GetWindow());
	instance.ChoosePhysicalDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, vk::YES);
	vk::Device device(instance.GetPhysicalDevice(), instance.GetInstance(), instance.GetSurface(), vk::GetValidationLayerNames());
	//////////////Texture load//////////////////////
	std::map<std::string, io::ImageData*> imageDataTable;
	imageDataTable.insert({ "floor01.jpg", io::LoadImage("floor01.jpg") });
	imageDataTable.insert({ "blackBrick.png", io::LoadImage("blackBrick.png") });
	imageDataTable.insert({ "brick.png", io::LoadImage("brick.png") });
	std::vector<io::ImageData*> texData(imageDataTable.size());
	std::transform(imageDataTable.begin(), imageDataTable.end(), texData.begin(),
		[](auto& x) {
			return x.second;
		});
	gGPUTextureManager = new components::GpuTextureManager(texData);
	
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
	auto bagulhoMeshData = io::LoadMeshes("bagulho.glb");
	components::Mesh* bagulhoMesh = new components::Mesh(bagulhoMeshData[0]);
	auto monkeyMeshData = io::LoadMeshes("monkey.glb");
	components::Mesh* monkeyMesh = new components::Mesh(monkeyMeshData[0]);
	auto sphereMeshData = io::LoadMeshes("sphere.glb");
	components::Mesh* sphereMesh = new components::Mesh(sphereMeshData[0]);
	//////////////Create the camera
	components::Camera* camera = new components::Camera("mainCamera");
	camera->mFOV = glm::radians(60.0f);
	camera->mRatio = (float)mainRenderPass.GetExtent().width / (float)mainRenderPass.GetExtent().height;
	camera->mZNear = 0.1f;
	camera->mZFar = 100.0f;
	camera->SetPosition({20, 10,20});
	camera->LookTo({ 0,0,0 });
	///////////Create the game objects
	//components::CameraTest* camera = new components::CameraTest();
	components::Renderable* myBagulho = new components::Renderable("myBagulho", *bagulhoMesh);
	myBagulho->SetPosition({ 0,0,0 });
	visibleObjects.push_back(myBagulho);

	components::Renderable* mySphere = new components::Renderable("mySphere", *sphereMesh);
	mySphere->SetPosition({ 0, -4, 0 });
	visibleObjects.push_back(mySphere);

	components::Renderable* myMonkey = new components::Renderable("myMonkey", *monkeyMesh);
	myMonkey->SetPosition({ 0,4,0 });
	visibleObjects.push_back(myMonkey);

	components::Renderable* myBox = new components::Renderable("MyBox", *boxMesh);
	myBox->SetPosition({ 4,0,0 });
	visibleObjects.push_back(myBox);

	components::Renderable* myBox2 = new components::Renderable("MyBox2", *boxMesh);
	myBox2->SetPosition({ -4,0,0 });
	myBox2->LookTo({ 100,100,0 });
	visibleObjects.push_back(myBox2);


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
		//camera->mRatio = (float)mainRenderPass.GetExtent().width / (float)mainRenderPass.GetExtent().height;
		commandBuffers = device.CreateCommandBuffers("mainCommandBuffer");
		};
	////////////OnRender
	size_t currentFrameId = 0;
	window.OnRender = [&currentFrameId, &commandBuffers, &shadowMapRenderPass, &mainRenderPass, 
		&phongPipeline, &syncService, &OnResize, &camera]
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
		for (auto& renderable : visibleObjects) {
			camera->Set(currentFrameId, *phongPipeline, frame.CommandBuffer());
			renderable->Set(currentFrameId, *phongPipeline, frame.CommandBuffer());
			phongPipeline->Draw(*renderable, frame.CommandBuffer()) ;

		}		
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