#include "vulkanapp/utils/DescriptorUtils.h"

#include <iostream>
#include <vector>
#include <stdexcept>

#include "vulkanapp/utils/ImageUtils.h"
#include "vulkanapp/utils/DescriptorUtils.h"
#include "structs/vulkan/PushConstantBlock.h"
#include "structs/vulkan/GPU_Buffer.h"

#include "vulkanapp/utils/MemoryUtils.h"
#include "vulkanapp/DeviceManager.h"

void utils::DescriptorUtils::setup_texture_descriptors(const vkb::DispatchTable& disp, const std::vector<Vk_Image>& textures, VkDescriptorSetLayout& outDescriptorSetLayout, VkDescriptorSet& outDescriptorSet, VkDescriptorPool& descriptor_pool)
{
    if (textures.empty())
    {
        std::cout << "No textures to load" << std::endl;
        return;    
    }
    
    //Descriptor Pool
    std::vector<VkDescriptorPoolSize> poolSizes = { descriptor_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(textures.size())) };
    VkDescriptorPoolCreateInfo descriptorPoolInfo = descriptor_pool_create_info(poolSizes, 1);
    
    disp.createDescriptorPool(&descriptorPoolInfo, nullptr, &descriptor_pool);

    //Descriptor set layout
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
    setLayoutBindings.push_back(descriptor_set_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, static_cast<uint32_t>(textures.size())));

    VkDescriptorSetLayoutBindingFlagsCreateInfo setLayoutBindingFlags{};
    setLayoutBindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    setLayoutBindingFlags.bindingCount = 1;

    // Enable variable descriptor count feature
    std::vector<VkDescriptorBindingFlags> descriptorBindingFlags =
    {
        0,
        VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT
    };
    setLayoutBindingFlags.pBindingFlags = descriptorBindingFlags.data();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI = descriptor_set_layout_create_info(setLayoutBindings);
    descriptorSetLayoutCI.pNext = &setLayoutBindingFlags;

    //@TODO: Make global
    VkDescriptorSetLayout descriptorSetLayout;
    disp.createDescriptorSetLayout(&descriptorSetLayoutCI, nullptr, &descriptorSetLayout);

    outDescriptorSetLayout = descriptorSetLayout;
    
    // We need to provide the descriptor counts for bindings with variable counts using a new structure
    std::vector<uint32_t> variableDescriptorCounts =
    {
        static_cast<uint32_t>(textures.size())
    };

    VkDescriptorSetVariableDescriptorCountAllocateInfo   variableDescriptorCountAllocInfo = {};
    variableDescriptorCountAllocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
    variableDescriptorCountAllocInfo.descriptorSetCount = static_cast<uint32_t>(variableDescriptorCounts.size());
    variableDescriptorCountAllocInfo.pDescriptorCounts  = variableDescriptorCounts.data();

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = descriptor_set_allocate_info(descriptor_pool, &descriptorSetLayout, 1);
    descriptorSetAllocateInfo.pNext = &variableDescriptorCountAllocInfo;

    VkDescriptorSet descriptorSet;
    disp.allocateDescriptorSets(&descriptorSetAllocateInfo, &descriptorSet);

    outDescriptorSet = descriptorSet;

    std::vector<VkWriteDescriptorSet> writeDescriptorSets(1);

    std::vector<VkDescriptorImageInfo> textureDescriptors(textures.size());
    for (size_t i = 0; i < textures.size(); i++)
    {
        textureDescriptors[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        textureDescriptors[i].sampler = textures[i].sampler;
        textureDescriptors[i].imageView = textures[i].view;
    }

    writeDescriptorSets[0] = {};
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].dstArrayElement = 0;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[0].descriptorCount = static_cast<uint32_t>(textures.size());
    writeDescriptorSets[0].pBufferInfo = 0;
    writeDescriptorSets[0].dstSet = descriptorSet;
    writeDescriptorSets[0].pImageInfo = textureDescriptors.data();

    disp.updateDescriptorSets(static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
}

// --- Example Usage (requires a Vulkan device and a created uniform buffer) ---
// This part is illustrative and requires a functional Vulkan context
/*
// Include necessary headers for the example main function if this was a standalone example
// #include <iostream> // For std::runtime_error

int main() {
    // Assume you have a VkDevice called 'device'
    // Assume you have a VkBuffer called 'uniformBuffer' with data for UniformBufferObject
    // Assume you have calculated the size of the buffer as 'uniformBufferSize'
    // VkDevice device = ...; // Your Vulkan device
    // VkBuffer uniformBuffer = ...; // Your uniform buffer
    // VkDeviceSize uniformBufferSize = sizeof(UniformBufferObject); // Size of your UBO

    try {
        // 1. Create the descriptor set layout
        // VkDescriptorSetLayout descriptorSetLayout = createDescriptorSetLayout(device);

        // 2. Create a descriptor pool
        // uint32_t maxSets = 1; // We only need one descriptor set in this simple example
        // VkDescriptorPool descriptorPool = createDescriptorPool(device, maxSets);

        // 3. Allocate and update the descriptor set
        // VkDescriptorSet descriptorSet = allocateAndWriteDescriptorSet(
        //    device,
        //    descriptorPool,
        //    descriptorSetLayout,
        //    uniformBuffer,
        //    uniformBufferSize);

        // Now you can use 'descriptorSetLayout' when creating your pipeline layout
        // and bind 'descriptorSet' in your command buffer before draw calls.

        // --- Cleanup (in your Vulkan cleanup process) ---
        // vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        // vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        // The uniformBuffer would also need to be destroyed and its memory freed.

    } catch (const std::runtime_error& e) {
        // Handle errors
        // std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
*/