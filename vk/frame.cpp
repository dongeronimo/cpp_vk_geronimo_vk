#include "frame.h"
#include <GLFW/glfw3.h>
#include <vk/synchronization_service.h>
#include <vk/instance.h>
#include <vk/device.h>
#include <vk/swap_chain.h>
#include <stdexcept>
#include <cassert>
#include <optional>
#include <chrono>
#include "app/imgui_utils.h"
std::optional<std::chrono::steady_clock::time_point> gLastFrameTime;
float gDeltaTime = 0;
namespace vk
{
    VkCommandBuffer Frame::CommandBuffer()
    {
        return commandBuffers[mCurrentFrame];
    }
    Frame::Frame(
        //static std::optional<
        const ring_buffer_t<VkCommandBuffer>& _commandBuffers,
        size_t currentFrame,
        vk::SyncronizationService& syncService,
        vk::SwapChain& swapChain) :
        mSyncService(syncService),
        mCurrentFrame(currentFrame),
        mSwapChain(swapChain),
        mImageIndex(UINT32_MAX),
        commandBuffers(_commandBuffers)
    {
        if (!gLastFrameTime) {
            gLastFrameTime = std::chrono::high_resolution_clock::now();
            gDeltaTime = 0;
        }
        else {
            auto currentTime = std::chrono::high_resolution_clock::now();
            gDeltaTime = std::chrono::duration<double>(
                currentTime - *gLastFrameTime).count();
            gLastFrameTime = currentTime;
        }
    }
    
    bool Frame::BeginFrame(app::ImguiUtils* imgui)
    {
        //auto inFlightFences = mSyncService.InFlightFences(mCurrentFrame);
        if (IsDegenerateFramebuffer())
            return false;
        //Fences block cpu, waiting for result. So we wait for the previous frame to finish
        vkWaitForFences(vk::Device::gDevice->GetDevice(), 1,
            &mSyncService.InFlightFences(mCurrentFrame),
            VK_TRUE, UINT64_MAX);
        vkResetFences(vk::Device::gDevice->GetDevice(), 1, &mSyncService.InFlightFences(mCurrentFrame));
        VkResult result = vkAcquireNextImageKHR(
            vk::Device::gDevice->GetDevice(),
            mSwapChain.GetSwapChain(), UINT64_MAX,
            mSyncService.ImagesAvailableSemaphore(mCurrentFrame), //this semaphore will be signalled when the presentation is done with this image
            VK_NULL_HANDLE, //no fence cares  
            &mImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            OnResize();
            return false;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
        //resets the command buffer
        vkResetCommandBuffer(CommandBuffer(), 0);
        //we must create imgui frame before beginning the command buffer.
        imgui->BeginImguiFrame();
        //begin the command buffer
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(CommandBuffer(), &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        
        return true;
    }

    void Frame::EndFrame()
    {
        auto commandBuffer = CommandBuffer();
        auto graphicsQueue = vk::Device::gDevice->GetGraphicsQueue();
        auto presentationQueue = vk::Device::gDevice->GetPresentationQueue();
        //end the command buffer
        if (vkEndCommandBuffer(CommandBuffer()) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
        //submits the queue:
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // it'll wait until the colours are written to the framebuffer
        std::vector<VkSemaphore> waitSemaphores{
            mSyncService.ImagesAvailableSemaphore(mCurrentFrame)
            /*,gpuPickerSemaphore*/
        };

        VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT /*| VK_PIPELINE_STAGE_TRANSFER_BIT*/;
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.pWaitDstStageMask = &waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;


        VkSemaphore signalSemaphores[] = { mSyncService.RenderFinishedSemaphore(mCurrentFrame) };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1,
            &submitInfo, mSyncService.InFlightFences(mCurrentFrame)) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }
        //presentation
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { mSwapChain.GetSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &mImageIndex;

        VkResult result = vkQueuePresentKHR(presentationQueue, &presentInfo);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            OnResize();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

    }
    float Frame::DeltaTime()
    {
        return gDeltaTime;
    }
    bool Frame::IsDegenerateFramebuffer() const
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(const_cast<GLFWwindow*>(
            vk::Instance::gInstance->mWindow), &width, &height);
        bool zeroArea = (width == 0) || (height == 0);
        if (zeroArea)
            return true;
        else return false;
    }
}