#include "lights.h"
#include "solid_phong_pipeline.h"
#include <vk/device.h>
namespace components {
    PointLightsUniform::PointLightsUniform()
    {
    }
    PointLightsUniform::~PointLightsUniform()
    {

    }
    void components::PointLightsUniform::Set(uint32_t currentFrame,
        const vk::Pipeline& pipeline, VkCommandBuffer cmdBuffer)
    {
        const auto device = vk::Device::gDevice->GetDevice();
        //get my pipeline
        const SolidPhongPipeline& phong = dynamic_cast<const SolidPhongPipeline&>(pipeline);
        void* data;
        vkMapMemory(device,
            phong.mPointLightsMemory[currentFrame],
            0, sizeof(PointLightsUniformBuffer), 0, &data);
        memcpy(data, &this->mUniformBufferData, sizeof(PointLightsUniformBuffer));
        vkUnmapMemory(device, phong.mPointLightsMemory[currentFrame]);
        ////CANT BIND IT HERE BECAUSE IT'S SHARED WITH CAMERA UNIFORM
        ////bind camera descriptor set
        //vkCmdBindDescriptorSets(cmdBuffer,
        //    VK_PIPELINE_BIND_POINT_GRAPHICS,
        //    phong.mPipelineLayout,
        //    0, //set 0 
        //    1,
        //    &phong.mCameraDescriptorSet[currentFrame], 0, nullptr);
    }
}
