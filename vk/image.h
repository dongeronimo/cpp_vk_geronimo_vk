#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include "device.h"
#include "instance.h"
namespace vk {
    void CreateImage(uint32_t width, uint32_t height, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties, VkImage& image,
        VkDeviceMemory& imageMemory);

    VkImageView CreateImageView(VkImage image, VkFormat format, 
        VkImageAspectFlags aspectFlags);

   
    class Texture {
    public:
        Texture(const std::string& filename);
        ~Texture();
        VkImageView GetImageView()const { return mImageView; }
        VkImage GetImage()const { return mImage; }
    private:
        VkImage mImage;
        VkDeviceMemory mMemory;//TODO memory: vkDeviceMemory are precious and i'll run out of if each texture creates it's own, the images should be in a big memory block with all the images
        VkImageView mImageView;
    };
}