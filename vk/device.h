#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <optional>
#include "data_structures/ring_buffer.h"
namespace vk {
    /// <summary>
    /// Holds the vulkan's device and it's queues. Should be initialized just
    /// once because the first time it's initialized it fills gDevice so that 
    /// i have easy access to it everywhere.
    /// </summary>
    class Device {
    public:
        static Device* gDevice;
        /// <summary>
        /// Creates the device
        /// - Fills gDevice.
        /// - Initializes the VkDevice, its queues and command pool
        /// - Initializes the object namer that is used for debugging
        /// </summary>
        /// <param name="physicalDevice"></param>
        /// <param name="instance"></param>
        /// <param name="surface"></param>
        /// <param name="validationLayers"></param>
        Device(VkPhysicalDevice physicalDevice, 
            VkInstance instance, 
            VkSurfaceKHR surface,
            std::vector<const char*> validationLayers);
        /// <summary>
        /// This destructor should be called after everything else but the 
        /// instance has been destroyed.
        /// </summary>
        ~Device();
        /// <summary>
        /// Get the main command pool.
        /// </summary>
        /// <returns></returns>
        VkCommandPool GetCommandPool()const { return mMainCommandPool; }
        /// <summary>
        /// Get VkDevice
        /// </summary>
        /// <returns></returns>
        VkDevice GetDevice()const { return mDevice; }
        /// <summary>
        /// The id of the graphics queue family. Depending on the GPU can be the same as the
        /// presentation queue family. This programa assumes they are both equal and if
        /// they are different it'll break due to the buffers using VK_SHARING_MODE_EXCLUSIVE 
        /// everywhere (see utils::CreateBuffer)
        /// </summary>
        /// <returns></returns>
        uint32_t GetGraphicsQueueFamily()const { return mGraphicsQueueFamily; }
        /// <summary>
        /// The if of the presentation queue family. Depending on the GPU it can be the same
        /// as the graphics queue family. This programa assumes they are both equal and if
        /// they are different it'll break due to the buffers using VK_SHARING_MODE_EXCLUSIVE 
        /// everywhere (see utils::CreateBuffer)
        /// </summary>
        /// <returns></returns>
        uint32_t GetPresentationQueueFamily()const { return mPresentationQueueFamily; }
        /// <summary>
        /// THe graphics queue.
        /// </summary>
        /// <returns></returns>
        VkQueue GetGraphicsQueue()const { return mGraphicsQueue; }
        /// <summary>
        /// The presentation queue.
        /// </summary>
        /// <returns></returns>
        VkQueue GetPresentationQueue()const { return mPresentationQueue; }
        /// <summary>
        /// Creates a single command buffer. THis command buffer will come from the short term commands pool, 
        /// that is transient
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        VkCommandBuffer CreateCommandBuffer(const std::string& name);
        /// <summary>
        /// Creates a ring buffer of command buffers. This command buffer will come from the main command pool
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        ring_buffer_t<VkCommandBuffer> CreateCommandBuffers(const std::string& name);
        /// <summary>
        /// Begin recording commands in a command buffer created with CreateCommandBuffer().
        /// </summary>
        /// <param name="cmd"></param>
        void BeginRecordingCommands(VkCommandBuffer cmd);
        /// <summary>
        /// Submit the command buffer created with CreateCommandBuffer().
        /// </summary>
        /// <param name="cmd"></param>
        void SubmitAndFinishCommands(VkCommandBuffer cmd);
        /// <summary>
        /// Copy data from a buffer to another. To do so it creates a command buffer, record the 
        /// commands and submit them.
        /// </summary>
        /// <param name="srcOffset"></param>
        /// <param name="dstOffset"></param>
        /// <param name="size"></param>
        /// <param name="cmd"></param>
        /// <param name="src"></param>
        /// <param name="dst"></param>
        void CopyBuffer(VkDeviceSize srcOffset, VkDeviceSize dstOffset,
            VkDeviceSize size, VkCommandBuffer cmd, VkBuffer src, VkBuffer dst);
    private:
        const VkPhysicalDevice mPhysicalDevice;
        const VkInstance mInstance;
        const VkSurfaceKHR mSurface;
        VkDevice mDevice = VK_NULL_HANDLE;
        VkQueue mGraphicsQueue = VK_NULL_HANDLE;
        VkQueue mPresentationQueue = VK_NULL_HANDLE;
        uint32_t mGraphicsQueueFamily = UINT32_MAX;
        uint32_t mPresentationQueueFamily = UINT32_MAX;
        VkCommandPool mMainCommandPool = VK_NULL_HANDLE;
        VkCommandPool mShortLivedCommandsPool = VK_NULL_HANDLE;
        std::optional<uint32_t> FindGraphicsQueueFamily(VkPhysicalDevice device);
        std::optional<uint32_t> FindPresentationQueueFamily(VkPhysicalDevice device, 
            VkSurfaceKHR surface);
    };
}