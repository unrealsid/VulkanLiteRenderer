#include "materials/Material.h"
#include "materials/ShaderObject.h"
#include "structs/engine/RenderContext.h"

#include "structs/vulkan/PushConstantBlock.h"
#include "vulkanapp/utils/DescriptorUtils.h"
#include "vulkanapp/utils/FileUtils.h"

namespace material
{
    Material::Material(std::string material_name, RenderContext* render_context) :
        material_name(std::move(material_name)), descriptor_set(nullptr), pipeline_layout(nullptr),
        render_context(render_context)
    {
        vertex_shader_path = R"(D:\Visual Studio 2022\Code\VulkanLiteRenderer\VkLiteRenderer\shaders\triangle.vert.spv)";
        fragment_shader_path = R"(D:\Visual Studio 2022\Code\VulkanLiteRenderer\VkLiteRenderer\shaders\triangle.frag.spv)";
    }

    void Material::add_shader_object(std::unique_ptr<ShaderObject> shader_object)
    {
        this->shader_object = std::move(shader_object);
    }

    void Material::add_pipeline_layout(VkPipelineLayout pipeline_layout)
    {
        this->pipeline_layout = pipeline_layout;
    }

    void Material::add_descriptor_set(VkDescriptorSet descriptor_set)
    {
        this->descriptor_set = descriptor_set;
    }

    Material Material::create_material(const std::string& name) const
    {
        //Setup push constants
        VkPushConstantRange push_constant_range{};
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset = 0;
        push_constant_range.size = sizeof(PushConstantBlock);

        size_t shaderCodeSizes[2]{};
        char* shaderCodes[2]{};

        utils::FileUtils::loadShader(vertex_shader_path, shaderCodes[0], shaderCodeSizes[0]);
        utils::FileUtils::loadShader(fragment_shader_path, shaderCodes[1], shaderCodeSizes[1]);

        auto shader_object = std::make_unique<ShaderObject>();
        shader_object->create_shaders(render_context->dispatch_table, shaderCodes[0], shaderCodeSizes[0], shaderCodes[1], shaderCodeSizes[1], nullptr, 0, nullptr, 0);

        VkPipelineLayout pipeline_layout;

        //Create the pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = utils::DescriptorUtils::pipeline_layout_create_info(nullptr,  0, nullptr, 0);
        render_context->dispatch_table.createPipelineLayout(&pipelineLayoutInfo, VK_NULL_HANDLE, &pipeline_layout);

        //Create material
        auto material = Material(name, render_context); //@TODO: Fix
        material.add_shader_object(std::move(shader_object));
        material.add_pipeline_layout(pipeline_layout);

        return material;
    }
}
