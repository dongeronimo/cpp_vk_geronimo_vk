#include "image.h"
#include <vulkan/vulkan.h>
#include <stdexcept>
#include "device.h"
#include "instance.h"
#include <utils/vk_utils.h>
#include "io/image-load.h"
#include "app/libdicom.h"
#include "debug_utils.h"
namespace vk {
    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
    {
        auto device = Device::gDevice->GetDevice();
        auto physicalDevice = Instance::gInstance->GetPhysicalDevice();
        // 1. Image creation
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image!");
        }

        // 2. Memory requirements
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        // 3. Allocate memory for the image
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = utils::FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate image memory!");
        }

        // 4. Bind memory to image
        vkBindImageMemory(device, image, imageMemory, 0);
    }
    void CreateImage(
        uint32_t width, uint32_t height, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image, VmaAllocationCreateFlags allocationFlags,
        VmaAllocation& allocation, VmaAllocationInfo& allocationInfo)
    {
        auto device = Device::gDevice->GetDevice();
        auto physicalDevice = Instance::gInstance->GetPhysicalDevice();
        auto& helper = mem::VmaHelper::GetInstance();
        // 1. Image creation
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional
        helper.CreateImage(imageInfo,
            properties, allocationFlags, image, allocation, allocationInfo);
        
    }
    void CreateImage(uint32_t width, uint32_t height, uint32_t depth, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VmaAllocationCreateFlags allocationFlags, VmaAllocation& allocation, VmaAllocationInfo& allocationInfo)
    {
        auto device = Device::gDevice->GetDevice();
        auto physicalDevice = Instance::gInstance->GetPhysicalDevice();
        auto& helper = mem::VmaHelper::GetInstance();
        // 1. Image creation
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_3D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = depth;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional
        helper.CreateImage(imageInfo,
            properties, allocationFlags, image, allocation, allocationInfo);

    }
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(Device::gDevice->GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view!");
        }

        return imageView;
    }

    Texture::~Texture() {
        VkDevice device = vk::Device::gDevice->GetDevice();
        vkDestroyImage(device, mImage, nullptr);
        mem::VmaHelper::GetInstance().FreeMemory(mImageAllocation);
        /*vkFreeMemory(device, mMemory, nullptr);*/
        vkDestroyImageView(device, mImageView, nullptr);
    }

    Texture::Texture(const std::string& filename)
    {
        VkDevice device = vk::Device::gDevice->GetDevice();
        //loads from file to ram
        auto filedata = io::LoadImage(filename);
        //from ram to buffer in gpu
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        utils::CreateBuffer(filedata->size,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT, //will be source of transfer
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,//must be seen and auto sychronized with the cpu memory
            stagingBuffer,
            stagingBufferMemory);
        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, filedata->size, 0, &data);
        memcpy(data, filedata->pixels.data(), filedata->size);
        vkUnmapMemory(device, stagingBufferMemory);
        CreateImage(filedata->w, filedata->h,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mImage,
            VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            mImageAllocation,
            mImageAllocationInfo);
        //change the image layout from undefined to destination-of-transfer
        utils::TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_SRGB, 
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        );

        VkCommandBuffer commandBuffer = vk::Device::gDevice->CreateCommandBuffer("image copy");
        vk::Device::gDevice->BeginRecordingCommands(commandBuffer);
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            unsigned(filedata->w),
            unsigned(filedata->h),
            1
        };
        vkCmdCopyBufferToImage(
            commandBuffer,
            stagingBuffer,
            mImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );
        vk::Device::gDevice->SubmitAndFinishCommands(commandBuffer);
        //now transition the image from a destination-of-transfer to shader read only, the layout expected by the shader for textures that'll be sampled
        utils::TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

        //now the imageview
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = mImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        vkCreateImageView(device, &viewInfo, nullptr, &mImageView);
    }

    Texture3d::Texture3d(volumerendering::LoadDicomImageResult& imageData)
    {
        size_t imageSizeInBytes = imageData.cols * imageData.rows * imageData.numberOfFiles * sizeof(short);
        VkPhysicalDevice physicalDevice = vk::Instance::gInstance->GetPhysicalDevice();
        VkDevice device = vk::Device::gDevice->GetDevice();
        mem::VmaHelper& helper = mem::VmaHelper::GetInstance();
        //create and fill the staging buffer
        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferAllocation;
        VmaAllocationInfo stagingBufferAllocationInfo;
        helper.CreateAlignedBuffer(imageSizeInBytes,
            utils::GetMinAlignment(physicalDevice),
            1,
            stagingBuffer,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            stagingBufferAllocation,
            stagingBufferAllocationInfo
        );
        memcpy(stagingBufferAllocationInfo.pMappedData, imageData.data, imageSizeInBytes);
        //create the image
        CreateImage(imageData.cols, imageData.rows, imageData.numberOfFiles,
            VK_FORMAT_R16_SINT,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mImage,
            VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
            mImageAllocation,
            mImageAllocationInfo);
        //copy from the staging buffer to the image
        utils::TransitionImageLayout(mImage, VK_FORMAT_R16_SINT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        );
        VkCommandBuffer commandBuffer = vk::Device::gDevice->CreateCommandBuffer("image copy");
        vk::Device::gDevice->BeginRecordingCommands(commandBuffer);
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            unsigned(imageData.cols),
            unsigned(imageData.rows),
            unsigned(imageData.numberOfFiles)
        };
        vkCmdCopyBufferToImage(
            commandBuffer,
            stagingBuffer,
            mImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );
        vk::Device::gDevice->SubmitAndFinishCommands(commandBuffer);
        //now transition the image from a destination-of-transfer to shader read only, the layout expected by the shader for textures that'll be sampled
        utils::TransitionImageLayout(mImage, VK_FORMAT_R16_SINT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
       
        //Create the imageview
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = mImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
        viewInfo.format = VK_FORMAT_R16_SINT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(Device::gDevice->GetDevice(), &viewInfo, nullptr, &mImageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view!");
        }
        SET_NAME(mImage, VK_OBJECT_TYPE_IMAGE, "volumeImage");
        SET_NAME(mImageView, VK_OBJECT_TYPE_IMAGE_VIEW, "volumeImageView");

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        helper.FreeMemory(stagingBufferAllocation);
    }
}