#pragma once

#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>
#include "materials/ShaderObject.h"

struct RenderContext;

namespace vkb
{
    struct DispatchTable;
}

namespace material
{
    class ShaderObject;
    
    class Material
    {

    public:
        Material() = default;
        Material(std::string material_name, RenderContext* render_context);

        //load the shader code
        void init();

        void add_shader_object(std::unique_ptr<ShaderObject> shader_object);
        void add_pipeline_layout(VkPipelineLayout pipeline_layout);
        void add_descriptor_set(VkDescriptorSet descriptor_set);

        VkPipelineLayout get_pipeline_layout() const { return pipeline_layout; }
        ShaderObject* get_shader_object() const { return shader_object.get(); }
        VkDescriptorSet& get_descriptor_set()  { return descriptor_set; }
        std::string get_material_name() const { return material_name; }

        Material create_material(const std::string& name) const;
    
    private: 
        std::unique_ptr<ShaderObject> shader_object;
        VkDescriptorSet descriptor_set;
        VkPipelineLayout pipeline_layout;

        RenderContext* render_context;

        std::string material_name;

        std::string vertex_shader_path;
        std::string fragment_shader_path;
    };
}