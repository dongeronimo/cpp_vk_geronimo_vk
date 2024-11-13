#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include "device.h"
#include "instance.h"
#include <mem/vma_helper.h>
namespace vk {
    /// <summary>
    /// Deprecated. Prefer the vma version.
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="format"></param>
    /// <param name="tiling"></param>
    /// <param name="usage"></param>
    /// <param name="properties"></param>
    /// <param name="image"></param>
    /// <param name="imageMemory"></param>
    void CreateImage(uint32_t width, uint32_t height, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties, VkImage& image,
        VkDeviceMemory& imageMemory);
    /// <summary>
    /// CreateImaage, but using VMA allocation instead of manually managing vkDeviceMemory
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="format"></param>
    /// <param name="tiling"></param>
    /// <param name="usage"></param>
    /// <param name="properties"></param>
    /// <param name="image"></param>
    /// <param name="allocation"></param>
    /// <param name="allocationInfo"></param>
    void CreateImage(uint32_t width, uint32_t height, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image, VmaAllocationCreateFlags allocationFlags,
        VmaAllocation& allocation, VmaAllocationInfo& allocationInfo);
    
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
        VmaAllocation mImageAllocation;
        VmaAllocationInfo mImageAllocationInfo;
        //VkDeviceMemory mMemory;//TODO memory: vkDeviceMemory are precious and i'll run out of if each texture creates it's own, the images should be in a big memory block with all the images
        VkImageView mImageView;
    };
}