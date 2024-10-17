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
#include "components/directional_light.h"

int main(int argc, char** argv)
{
	app::Window window(SCREEN_WIDTH, SCREEN_HEIGH);
	
	vk::Instance instance(window.GetWindow());
	instance.ChoosePhysicalDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, vk::YES);
	vk::Device device(instance.GetPhysicalDevice(), instance.GetInstance(), instance.GetSurface(), vk::GetValidationLayerNames());
	//////////////create render passes//////////////
	components::MainRenderPass mainRenderPass;
	components::ShadowMapRenderPass shadowMapRenderPass(DIRECTIONAL_SHADOW_MAP_SIZE, DIRECTIONAL_SHADOW_MAP_SIZE, mainRenderPass.GetNumberOfSwapChainColorAttachments());
	//create pipelines
	components::DirectionalLightShadowMapPipeline* directionaLightShadowMapPipeline = new components::DirectionalLightShadowMapPipeline(shadowMapRenderPass);
	components::SolidPhongPipeline* phongPipeline = new components::SolidPhongPipeline(mainRenderPass, shadowMapRenderPass.GetShadowBufferImageViews());
	//create synchronization objects
	vk::SyncronizationService syncService;
	///////////////load meshes
	auto boxMeshData = io::LoadMeshes("box.glb");
	auto floorMeshData = io::LoadMeshes("floor.glb");
	components::Mesh* boxMesh = new components::Mesh(boxMeshData[0]);//there can be many meshes per file, i know that in this file there's only one.
	components::Mesh* floorMesh = new components::Mesh(floorMeshData[0]);
	//////////////Create the camera
	components::Camera* camera = new components::Camera("mainCamera");
	camera->mFOV = glm::radians(60.0f);
	camera->mRatio = (float)mainRenderPass.GetExtent().width / (float)mainRenderPass.GetExtent().height;
	camera->mZNear = 0.1f;
	camera->mZFar = 100.0f;
	camera->mPosition = { 10, 5,0};
	camera->LookTo({ 0,0,0 });
	/////////////Create the game objects
	components::Renderable* myBox = new components::Renderable("MyBox", *boxMesh);
	myBox->mPosition = { 0,0,0 };
	myBox->LookTo({ 1,1,0 });

	components::Renderable* myBox2 = new components::Renderable("MyBox2", *boxMesh);
	myBox2->mPosition = { 5,0,0 };
	myBox2->LookTo({ 1,0,0 });
	std::vector<components::Renderable*> shadowProjectors{ myBox, myBox2};
	std::vector<components::Renderable*> phongPipelineObjects{ myBox, myBox2};
	for (auto i = -5; i < 5; i++) {
		for (auto j = -5; j < 5; j++) {
			components::Renderable* myFloor = new components::Renderable("Floor", *floorMesh);
			myFloor->mPosition = { i*2, -2,j*2 };
			shadowProjectors.push_back(myFloor);
			phongPipelineObjects.push_back(myFloor);
		}
	}

	components::DirectionalLight* myDirectionalLight = new components::DirectionalLight();
	myDirectionalLight->mPosition = { 0,0,0 };
	myDirectionalLight->LookTo({ 0,-1,0 });
	////////////Create the command buffer
	ring_buffer_t<VkCommandBuffer> commandBuffers = device.CreateCommandBuffers("mainCommandBuffer");
	////////////On Resize
	std::function<void()> OnResize = [ &device, &mainRenderPass, &commandBuffers, &phongPipeline, &camera, &window]() {
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
	size_t currentFrameId = 0;
	window.OnRender = [&myDirectionalLight, &currentFrameId, &commandBuffers, &shadowMapRenderPass, &mainRenderPass,
		&phongPipeline, &syncService, &camera, &shadowProjectors,&phongPipelineObjects, &OnResize, &directionaLightShadowMapPipeline]
	(app::Window* wnd) {
		//begin the frame
		vk::Frame frame(commandBuffers,currentFrameId, syncService, *mainRenderPass.GetSwapChain());
		frame.OnResize = OnResize;
		frame.BeginFrame();
		//Shadow map: activate the render passes
		shadowMapRenderPass.SetImageIndex(frame.mImageIndex);
		shadowMapRenderPass.BeginRenderPass(frame.CommandBuffer(), frame.mImageIndex, currentFrameId);

		//Shadow map: activate pipelines that use the render pass
		directionaLightShadowMapPipeline->Bind(frame.CommandBuffer(), currentFrameId);
		//glm::vec3 lightDirection = { 1,0,1 };
		//glm::vec3 lightPos = -lightDirection * 30.0f; //TODO light: do not use hardcoded distance
		//glm::vec3 lightTarget = { 0,0,0 };//TODO light: calculate the center of the visible objects, based on the frustum
		//glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, {0,1,0});
		//glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.f);//TODO light: calculate based on the objects visible on the frustum
		//glm::mat4 lightMatrix = lightProj * lightView;
		components::LightSpaceMatrixUniformBuffer lightMatrixObj{ myDirectionalLight->GetLightMatrix()};
		directionaLightShadowMapPipeline->SetLightMatrix(frame.CommandBuffer(), lightMatrixObj);
		for (auto& o : shadowProjectors) {
			o->SetUniforms(currentFrameId, *directionaLightShadowMapPipeline, frame.CommandBuffer());
			directionaLightShadowMapPipeline->Draw(*o, frame.CommandBuffer());
		}
		directionaLightShadowMapPipeline->Unbind(frame.CommandBuffer());
		//Shadow map: end the render pass
		shadowMapRenderPass.EndRenderPass(frame.CommandBuffer());
		//main render pass: activate the render pass
		mainRenderPass.BeginRenderPass(frame.CommandBuffer(), frame.mImageIndex, currentFrameId);
		mainRenderPass.SetImageIndex(frame.mImageIndex);
		//activate pipelines that use the render pass
		phongPipeline->Bind(frame.CommandBuffer(), currentFrameId);
		myDirectionalLight->SetUniform(currentFrameId, *phongPipeline, frame.CommandBuffer());
		phongPipeline->ActivateShadowMap(frame.mImageIndex, frame.CommandBuffer());
		camera->SetUniform(currentFrameId, *phongPipeline, frame.CommandBuffer());
		for (auto& o : phongPipelineObjects)
		{
			o->SetUniforms(currentFrameId, *phongPipeline, frame.CommandBuffer());
			phongPipeline->Draw(*o, frame.CommandBuffer());
		}
		phongPipeline->Unbind(frame.CommandBuffer());
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
	delete directionaLightShadowMapPipeline;
	return 0;
}