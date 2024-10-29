#pragma once
#if MAX_FRAMES_IN_FLIGHT <= 0
#error MAX_FRAMES_IN_FLIGHT MUST BE GREATER THEN ZERO
#endif
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
namespace vk {
    enum PhysicalDeviceFeatureQueryParam {
        DONT_CARE,
        YES
    };
    /// <summary>
    /// Holds the properties and features of a physical device. These objects
    /// are created by the Instance's ctor for us to choose a physical device
    /// for the rest of the app's execution.
    /// </summary>
    class PhysicalDeviceProperties {
    public:
        PhysicalDeviceProperties(const PhysicalDeviceProperties& o):
            mProperties(o.mProperties), mFeatures(o.mFeatures), mDevice(o.mDevice), 
            mName(o.mName){}
        PhysicalDeviceProperties(VkPhysicalDeviceProperties p, 
            VkPhysicalDeviceFeatures f,
            VkPhysicalDevice d)
            :mProperties(p), mFeatures(f), mDevice(d), mName(p.deviceName)
        {
        }
        const VkPhysicalDeviceProperties mProperties;
        const VkPhysicalDeviceFeatures mFeatures;
        const VkPhysicalDevice mDevice;
        const std::string mName;
    };
    /// <summary>
    /// Holds the vulkan instance and the physical device. After you instantiate the 
    /// object it'll be available everywhere using the gInstance static member. Also,
    /// after the instantiation, you have to call ChoosePhysicalDevice with the parameters
    /// for the kind of device you want
    /// </summary>
    class Instance {
    public:
        /// <summary>
        /// Singleton initialized when the ctor is called for the first time.
        /// </summary>
        static Instance* gInstance;
        /// <summary>
        /// Call this ctor just once because it'll fill gInstance.
        /// Creates the VkInstance, the debug messenger, and get the physical devices.
        /// It'll not choose the physical device, that has to be done later.
        /// </summary>
        /// <param name="window"></param>
        Instance(GLFWwindow* window);
        /// <summary>
        /// Pointer to the glfw window. 
        /// TODO multiplatform: that'll have to decoupled from glfw if i try to put it in another platform.
        /// </summary>
        const GLFWwindow* mWindow;
        /// <summary>
        /// Destructor. Has to be the last vulkan object destructor called.
        /// </summary>
        ~Instance();
        /// <summary>
        /// Returns the vkInstance
        /// </summary>
        /// <returns></returns>
        VkInstance GetInstance()const { return mInstance; }
        /// <summary>
        /// Returns the vkSurface
        /// </summary>
        /// <returns></returns>
        VkSurfaceKHR GetSurface()const { return mSurface; }
        /// <summary>
        /// Get the physical device. Will crash if called before ChoosePhysicalDevice
        /// </summary>
        /// <returns></returns>
        VkPhysicalDevice GetPhysicalDevice()const;
        /// <summary>
        /// Call this after the ctor and before GetPhysicalDevice
        /// </summary>
        /// <param name="type"></param>
        /// <param name="samplerAnisotropy"></param>
        void ChoosePhysicalDevice(VkPhysicalDeviceType type, PhysicalDeviceFeatureQueryParam samplerAnisotropy);
    private:
        /// <summary>
        /// Which device was chosen?
        /// </summary>
        uint32_t mChosenDeviceId = UINT32_MAX;
        /// <summary>
        /// Table of physical device properties
        /// </summary>
        std::vector<PhysicalDeviceProperties> mPhysicalDevices;
        /// <summary>
        /// vk instance
        /// </summary>
        VkInstance mInstance = VK_NULL_HANDLE;
        /// <summary>
        /// vk debug handler
        /// </summary>
        VkDebugUtilsMessengerEXT mDebugMessager = VK_NULL_HANDLE;
        /// <summary>
        /// vk surface
        /// </summary>
        VkSurfaceKHR mSurface = VK_NULL_HANDLE;
        void CreateInstance();
    };
}