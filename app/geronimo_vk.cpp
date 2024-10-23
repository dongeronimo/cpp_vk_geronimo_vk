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
#include "vk/image.h"
#include "components/lights.h"
components::GpuTextureManager* gGPUTextureManager = nullptr;
components::PointLightsUniform* gPointLights = nullptr;
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
	VkSampler linearRepeatSampler = vk::MakeLinearRepeat2DSampler("solidPhongTextureSampler");
	components::SolidPhongPipeline* phongBrickPipeline = new components::SolidPhongPipeline(
		"brick.png_phong_pipeline",
		mainRenderPass, 
		linearRepeatSampler,
		gGPUTextureManager->GetImageView("brick.png"));
	components::SolidPhongPipeline* phongBlackBrickPipeline = new components::SolidPhongPipeline(
		"blackBrick.png_phong_pipeline",
		mainRenderPass,
		linearRepeatSampler,
		gGPUTextureManager->GetImageView("blackBrick.png"));
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
	camera->SetPosition({10, 5,10});
	camera->LookTo({ 0,0,0 });
	///////////Create the game objects
	//components::CameraTest* camera = new components::CameraTest();
	components::Renderable* myBagulho = new components::Renderable("myBagulho", *bagulhoMesh);
	myBagulho->SetPosition({ 0,0,0 });
	myBagulho->mMaterial.SetDiffuseColor({ 1,0,0 }, 1);
	phongBrickPipeline->AddRenderable(myBagulho);

	components::Renderable* mySphere = new components::Renderable("mySphere", *sphereMesh);
	mySphere->SetPosition({ 0, -4, 0 });
	myBagulho->mMaterial.SetDiffuseColor({ 1,1,0 }, 1);
	phongBrickPipeline->AddRenderable(mySphere);

	components::Renderable* myMonkey = new components::Renderable("myMonkey", *monkeyMesh);
	myMonkey->SetPosition({ 0,4,0 });
	myMonkey->mMaterial.SetDiffuseColor({ 1,0,1 }, 1);
	phongBrickPipeline->AddRenderable(myMonkey);

	components::Renderable* myBox = new components::Renderable("MyBox", *boxMesh);
	myBox->SetPosition({ 4,0,0 });
	myBox->mMaterial.SetDiffuseColor({ 0,1,0 },1);
	phongBlackBrickPipeline->AddRenderable(myBox);

	components::Renderable* myBox2 = new components::Renderable("MyBox2", *boxMesh);
	myBox2->SetPosition({ -4,0,0 });
	myBox2->mMaterial.SetDiffuseColor({ 0,0,1 }, 1);
	myBox2->LookTo({ 100,100,0 });
	phongBlackBrickPipeline->AddRenderable(myBox2);

	gPointLights = new components::PointLightsUniform();
	
	gPointLights->SetLightActive(0, true);
	gPointLights->SetLightColorAndIntensity(0, { 1,1,1 }, 1);
	gPointLights->SetLightPosition(0, glm::vec3{ 10,0,0 });

	gPointLights->SetLightActive(1, true);
	gPointLights->SetLightColorAndIntensity(1, { 1,1,1 }, 0.5f);
	gPointLights->SetLightPosition(1, glm::vec3{ 0, 10, 0 });
	////////////Create the command buffer
	ring_buffer_t<VkCommandBuffer> commandBuffers = device.CreateCommandBuffers("mainCommandBuffer");
	////////////On Resize
	std::function<void()> OnResize = [&device, &mainRenderPass, &commandBuffers, &phongBrickPipeline, &phongBlackBrickPipeline, &camera, &window]() {
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
		phongBrickPipeline->DestroyPipeline();
		phongBlackBrickPipeline->DestroyPipeline();
		mainRenderPass.DestroyRenderPass();
		mainRenderPass.GetSwapChain()->DestroyImageViews();
		mainRenderPass.GetSwapChain()->DestroySwapChain();
		//recreate them
		mainRenderPass.Recreate();
		phongBrickPipeline->Recreate();
		phongBlackBrickPipeline->Recreate();
		//camera->mRatio = (float)mainRenderPass.GetExtent().width / (float)mainRenderPass.GetExtent().height;
		commandBuffers = device.CreateCommandBuffers("mainCommandBuffer");
		};
	////////////OnRender
	size_t currentFrameId = 0;
	window.OnRender = [&currentFrameId, &commandBuffers, &shadowMapRenderPass, &mainRenderPass, 
		&phongBrickPipeline, &syncService, &OnResize, &camera, &phongBlackBrickPipeline]
	(app::Window* wnd) {
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
		camera->Set(currentFrameId, *phongBrickPipeline, frame.CommandBuffer());
		gPointLights->Set(currentFrameId, *phongBrickPipeline, frame.CommandBuffer());
		phongBrickPipeline->Bind(frame.CommandBuffer(), currentFrameId);
		auto& phongBrickObjs = phongBrickPipeline->GetRenderables();
		for (auto& renderable : phongBrickObjs) {
			if (renderable == nullptr)
				continue;
			renderable->mMaterial.Set(currentFrameId, *phongBrickPipeline, frame.CommandBuffer());
			renderable->Set(currentFrameId, *phongBrickPipeline, frame.CommandBuffer());
			phongBrickPipeline->Draw(*renderable, frame.CommandBuffer(), currentFrameId) ;
		}		

		camera->Set(currentFrameId, *phongBlackBrickPipeline, frame.CommandBuffer());
		gPointLights->Set(currentFrameId, *phongBlackBrickPipeline, frame.CommandBuffer());
		phongBlackBrickPipeline->Bind(frame.CommandBuffer(), currentFrameId);
		auto& phongBlacBrickObjs = phongBlackBrickPipeline->GetRenderables();
		for (auto& renderable : phongBlacBrickObjs) {
			if (renderable == nullptr)
				continue;
			renderable->mMaterial.Set(currentFrameId, *phongBlackBrickPipeline, frame.CommandBuffer());
			renderable->Set(currentFrameId, *phongBlackBrickPipeline, frame.CommandBuffer());
			phongBlackBrickPipeline->Draw(*renderable, frame.CommandBuffer(), currentFrameId);
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
	delete gGPUTextureManager;
	delete myBox;
	delete boxMesh;
	delete camera;
	delete phongBrickPipeline;
	return 0;
}