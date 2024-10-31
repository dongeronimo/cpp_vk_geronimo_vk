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
#include <utils/concatenate.h>
#include "components/animation.h"
#include <memory>

std::vector<components::Renderable*> gObjectsList;
int main(int argc, char** argv)
{
	//Create the window
	app::Window window(SCREEN_WIDTH, SCREEN_HEIGH);
	//create the instance and choose the physical device, i want the discrete gpu
	vk::Instance instance(window.GetWindow());
	instance.ChoosePhysicalDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, vk::YES);
	//create the device
	vk::Device device(instance.GetPhysicalDevice(), instance.GetInstance(), instance.GetSurface(), vk::GetValidationLayerNames());
	////////////////create render passes//////////////
	components::MainRenderPass mainRenderPass;
	components::ShadowMapRenderPass shadowMapRenderPass(DIRECTIONAL_SHADOW_MAP_SIZE, DIRECTIONAL_SHADOW_MAP_SIZE, mainRenderPass.GetNumberOfSwapChainColorAttachments());
	////create pipelines
	components::DirectionalLightShadowMapPipeline* directionaLightShadowMapPipeline = new components::DirectionalLightShadowMapPipeline(shadowMapRenderPass);
	components::SolidPhongPipeline* phongPipeline = new components::SolidPhongPipeline(mainRenderPass, shadowMapRenderPass.GetShadowBufferImageViews());
	////create synchronization objects
	vk::SyncronizationService syncService;
	/////////////////load meshes
	auto floorMeshData = io::LoadMeshes("floor.glb");
	auto monkeyMeshData = io::LoadMeshes("monkey.glb");
	auto boxMeshData = io::LoadMeshes("untitled.glb");
	auto coneMeshData = io::LoadMeshes("cone.glb");
	components::Mesh* floorMesh = new components::Mesh(floorMeshData[0]);
	components::Mesh* monkeyMesh = new components::Mesh(monkeyMeshData[0]);
	components::Mesh* boxMesh = new components::Mesh(boxMeshData[0]);
	components::Mesh* coneMesh = new components::Mesh(coneMeshData[0]);
	////////////////Create the camera
	components::Camera* camera = new components::Camera("mainCamera");
	camera->mFOV = glm::radians(45.0f);
	camera->mRatio = (float)mainRenderPass.GetExtent().width / (float)mainRenderPass.GetExtent().height;
	camera->mZNear = 0.01f;
	camera->mZFar = 100.0f;
	camera->mPosition = { 5, 5, 5};
	camera->LookTo({ 0,0,0 });
	///////////////Create the game objects
	components::Renderable* myBox1 = new components::Renderable("box1", *boxMesh);
	myBox1->mPosition = {-3,0,0};
	myBox1->EnqueueAnimation(std::make_shared<components::animations::RotateAroundForever>(glm::vec3(0, 1, 0), 90.0f, myBox1));
	directionaLightShadowMapPipeline->AddRenderable(myBox1);
	phongPipeline->AddRenderable(myBox1);
	gObjectsList.push_back(myBox1);


	components::Renderable* myMonkey = new components::Renderable("monkey", *monkeyMesh);
	myMonkey->mPosition = { 0,0,0};
	myMonkey->LookTo({ 100,0,0 });
	directionaLightShadowMapPipeline->AddRenderable(myMonkey);
	phongPipeline->AddRenderable(myMonkey);
	gObjectsList.push_back(myMonkey);

	components::Renderable* myBox2 = new components::Renderable("box2", *boxMesh);
	myBox2->mPosition = { 3,0,0 };
	directionaLightShadowMapPipeline->AddRenderable(myBox2);
	phongPipeline->AddRenderable(myBox2);
	gObjectsList.push_back(myBox2);

	components::Renderable* myCone = new components::Renderable("cone", *coneMesh);
	myCone->mPosition = { 0, 0, 3 };
	directionaLightShadowMapPipeline->AddRenderable(myCone);
	phongPipeline->AddRenderable(myCone);
	gObjectsList.push_back(myCone);

	components::DirectionalLight* myDirectionalLight = new components::DirectionalLight();
	myDirectionalLight->SetColor({ 1,1,1 });
	myDirectionalLight->SetIntensity(1.0f);

	//////////////Create the command buffer
	ring_buffer_t<VkCommandBuffer> commandBuffers = device.CreateCommandBuffers("mainCommandBuffer");
	//////////////On Resize
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
	//////////////OnRender
	size_t currentFrameId = 0;
	window.OnRender = [&myDirectionalLight, &currentFrameId, &commandBuffers, &shadowMapRenderPass, &mainRenderPass,
		&phongPipeline, &syncService, &camera, &OnResize, &directionaLightShadowMapPipeline, &myBox2]
	(app::Window* wnd) {
		//begin the frame
		vk::Frame frame(commandBuffers,currentFrameId, syncService, *mainRenderPass.GetSwapChain());
		frame.OnResize = OnResize;
		frame.BeginFrame();
		///animations
		for (auto& o : gObjectsList) {
			o->AdvanceAnimation(frame.DeltaTime());
		}
		//float deltaT = frame.DeltaTime();
		//static float angle = 0.0f;
		//angle += 90.0f * deltaT;
		//glm::quat dest = glm::angleAxis(glm::radians(angle), glm::vec3{ 0,1,0 });
		//myBox2->mOrientation = glm::lerp(myBox2->mOrientation, dest, 0.5f);

		//Shadow map: activate the render passes
		shadowMapRenderPass.SetImageIndex(frame.mImageIndex);
		shadowMapRenderPass.BeginRenderPass(frame.CommandBuffer(), frame.mImageIndex, currentFrameId);

		//Shadow map: activate pipelines that use the render pass
		directionaLightShadowMapPipeline->Bind(frame.CommandBuffer(), currentFrameId);

		components::LightSpaceMatrixUniformBuffer lightMatrixObj{ myDirectionalLight->GetLightMatrix()};
		directionaLightShadowMapPipeline->SetLightMatrix(frame.CommandBuffer(), lightMatrixObj);
		;
		for (auto& o : directionaLightShadowMapPipeline->GetRenderables()) {
			if (o != nullptr) {
				o->SetUniforms(currentFrameId, *directionaLightShadowMapPipeline, frame.CommandBuffer());
				directionaLightShadowMapPipeline->Draw(*o, frame.CommandBuffer());
			}
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
		for (auto& o : phongPipeline->GetRenderables())
		{
			if (o != nullptr) {
				o->SetUniforms(currentFrameId, *phongPipeline, frame.CommandBuffer());
				phongPipeline->Draw(*o, frame.CommandBuffer());
			}
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
	syncService.WaitDeviceIdle(); //wait for everything

	delete phongPipeline;
	delete directionaLightShadowMapPipeline;
	delete floorMesh;
	delete monkeyMesh;
	delete boxMesh;
	delete coneMesh;
	delete camera;
	delete myBox1;
	delete myBox2;
	delete myCone;
	delete myMonkey;
	return 0;
}